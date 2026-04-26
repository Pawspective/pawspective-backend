import uuid
import pytest


def make_unique_email(prefix='animal_list'):
    random_part = uuid.uuid4().hex[:10]
    return f'{prefix}_{random_part}@example.com'


@pytest.fixture
async def city(pgsql):
    conn = pgsql['postgres-db']
    cursor = conn.cursor()
    city_name = f'TestCity_{uuid.uuid4().hex[:8]}'
    cursor.execute(
        'INSERT INTO cities (name) VALUES (%s) RETURNING id, name',
        (city_name,),
    )
    row = cursor.fetchone()
    return {'id': row[0], 'name': row[1]}


@pytest.fixture
async def dog_breed(pgsql):
    conn = pgsql['postgres-db']
    cursor = conn.cursor()
    breed_name = f'DogBreed_{uuid.uuid4().hex[:8]}'
    cursor.execute(
        'INSERT INTO breeds (animal_type, name) VALUES (%s, %s) RETURNING id, animal_type, name',
        ('dog', breed_name),
    )
    row = cursor.fetchone()
    return {'id': row[0], 'animal_type': row[1], 'name': row[2]}


@pytest.fixture
async def cat_breed(pgsql):
    conn = pgsql['postgres-db']
    cursor = conn.cursor()
    breed_name = f'CatBreed_{uuid.uuid4().hex[:8]}'
    cursor.execute(
        'INSERT INTO breeds (animal_type, name) VALUES (%s, %s) RETURNING id, animal_type, name',
        ('cat', breed_name),
    )
    row = cursor.fetchone()
    return {'id': row[0], 'animal_type': row[1], 'name': row[2]}


@pytest.fixture
async def authenticated_user(service_client):
    email = make_unique_email('owner')
    password = 'TestPassword123'

    register_response = await service_client.post(
        '/user/register',
        json={
            'email': email,
            'password': password,
            'first_name': 'Owner',
            'last_name': 'User',
        },
    )
    assert register_response.status == 201

    login_response = await service_client.post(
        '/auth/login',
        json={'email': email, 'password': password},
    )
    assert login_response.status == 200
    tokens = login_response.json()
    return {'token': tokens['access_token']}


@pytest.fixture
async def registered_org(service_client, authenticated_user, city):
    response = await service_client.post(
        '/orgs',
        json={
            'name': f'Shelter_{uuid.uuid4().hex[:8]}',
            'city_id': city['id'],
        },
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )
    assert response.status == 201
    return response.json()


async def create_animal(
    service_client, token, org_id, breed_id, overrides=None
):
    payload = {
        'name': f'Animal_{uuid.uuid4().hex[:6]}',
        'organization_id': org_id,
        'breed_id': breed_id,
        'size': 'medium',
        'gender': 'male',
        'care_level': 'easy',
        'color': 'black',
        'good_with': 'dogs',
        'age': 3,
        'status': 'available',
    }
    if overrides:
        payload.update(overrides)
    response = await service_client.post(
        '/animals',
        json=payload,
        headers={'Authorization': f'Bearer {token}'},
    )
    assert response.status == 201
    return response.json()


async def test_animal_list_returns_200(service_client):
    """GET /animals returns 200 without auth"""
    response = await service_client.get('/animals')
    assert response.status == 200


async def test_animal_list_returns_array(service_client):
    """GET /animals returns a JSON array"""
    response = await service_client.get('/animals')
    assert response.status == 200
    assert isinstance(response.json(), list)


async def test_animal_list_no_auth_required(service_client):
    """GET /animals is publicly accessible"""
    response = await service_client.get('/animals')
    assert response.status == 200


async def test_animal_list_contains_created_animal(
    service_client, authenticated_user, registered_org, dog_breed
):
    """Animals that were registered appear in the list"""
    animal = await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        dog_breed['id'],
    )

    response = await service_client.get('/animals')
    assert response.status == 200
    data = response.json()
    assert any(a['id'] == animal['id'] for a in data)


async def test_animal_list_response_shape(
    service_client, authenticated_user, registered_org, dog_breed
):
    """Each animal in the list has the expected fields"""
    animal = await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        dog_breed['id'],
        {'name': 'Shaper', 'color': 'brown', 'age': 2},
    )

    response = await service_client.get('/animals')
    assert response.status == 200
    data = response.json()
    item = next(a for a in data if a['id'] == animal['id'])
    assert 'id' in item
    assert 'name' in item
    assert 'organization_id' in item
    assert 'breed' in item
    assert 'size' in item
    assert 'gender' in item
    assert 'care_level' in item
    assert 'color' in item
    assert 'good_with' in item
    assert 'age' in item
    assert 'status' in item


async def test_animal_list_filter_by_size(
    service_client, authenticated_user, registered_org, dog_breed
):
    """Filter by size returns only matching animals"""
    small = await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        dog_breed['id'],
        {'size': 'small'},
    )
    large = await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        dog_breed['id'],
        {'size': 'large'},
    )

    response = await service_client.get('/animals?sizes=small')
    assert response.status == 200
    data = response.json()
    ids = [a['id'] for a in data]
    assert small['id'] in ids
    assert large['id'] not in ids


async def test_animal_list_filter_by_gender(
    service_client, authenticated_user, registered_org, dog_breed
):
    """Filter by gender returns only matching animals"""
    male = await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        dog_breed['id'],
        {'gender': 'male'},
    )
    female = await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        dog_breed['id'],
        {'gender': 'female'},
    )

    response = await service_client.get('/animals?genders=female')
    assert response.status == 200
    data = response.json()
    ids = [a['id'] for a in data]
    assert female['id'] in ids
    assert male['id'] not in ids


async def test_animal_list_filter_by_color(
    service_client, authenticated_user, registered_org, dog_breed
):
    """Filter by color returns only matching animals"""
    white = await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        dog_breed['id'],
        {'color': 'white'},
    )
    grey = await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        dog_breed['id'],
        {'color': 'grey'},
    )

    response = await service_client.get('/animals?colors=white')
    assert response.status == 200
    data = response.json()
    ids = [a['id'] for a in data]
    assert white['id'] in ids
    assert grey['id'] not in ids


async def test_animal_list_filter_by_age_range(
    service_client, authenticated_user, registered_org, dog_breed
):
    """Filter by age_gte and age_lte returns animals within the range"""
    young = await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        dog_breed['id'],
        {'age': 1},
    )
    old = await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        dog_breed['id'],
        {'age': 10},
    )

    response = await service_client.get('/animals?age_gte=1&age_lte=3')
    assert response.status == 200
    data = response.json()
    ids = [a['id'] for a in data]
    assert young['id'] in ids
    assert old['id'] not in ids


async def test_animal_list_filter_by_breed(
    service_client, authenticated_user, registered_org, dog_breed, cat_breed
):
    """Filter by breed_id returns only animals of that breed"""
    dog = await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        dog_breed['id'],
    )
    cat = await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        cat_breed['id'],
    )

    response = await service_client.get(f'/animals?breeds={dog_breed["id"]}')
    assert response.status == 200
    data = response.json()
    ids = [a['id'] for a in data]
    assert dog['id'] in ids
    assert cat['id'] not in ids


async def test_animal_list_filter_no_match_returns_empty(
    service_client, authenticated_user, registered_org, dog_breed
):
    """Filters that match nothing return an empty list"""
    await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        dog_breed['id'],
        {'age': 5},
    )

    response = await service_client.get('/animals?age_gte=100&age_lte=200')
    assert response.status == 200
    data = response.json()
    assert isinstance(data, list)
    assert len(data) == 0


async def test_animal_list_filter_size_and_gender(
    service_client, authenticated_user, registered_org, dog_breed
):
    """Combining size and gender filters returns only animals matching both"""
    match = await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        dog_breed['id'],
        {'size': 'small', 'gender': 'female'},
    )
    wrong_gender = await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        dog_breed['id'],
        {'size': 'small', 'gender': 'male'},
    )
    wrong_size = await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        dog_breed['id'],
        {'size': 'large', 'gender': 'female'},
    )

    response = await service_client.get('/animals?sizes=small&genders=female')
    assert response.status == 200
    data = response.json()
    ids = [a['id'] for a in data]
    assert match['id'] in ids
    assert wrong_gender['id'] not in ids
    assert wrong_size['id'] not in ids


async def test_animal_list_filter_color_and_age(
    service_client, authenticated_user, registered_org, dog_breed
):
    """Combining color and age range filters works correctly"""
    match = await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        dog_breed['id'],
        {'color': 'golden', 'age': 2},
    )
    wrong_color = await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        dog_breed['id'],
        {'color': 'brown', 'age': 2},
    )
    wrong_age = await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        dog_breed['id'],
        {'color': 'golden', 'age': 8},
    )

    response = await service_client.get(
        '/animals?colors=golden&age_gte=1&age_lte=3'
    )
    assert response.status == 200
    data = response.json()
    ids = [a['id'] for a in data]
    assert match['id'] in ids
    assert wrong_color['id'] not in ids
    assert wrong_age['id'] not in ids


async def test_animal_list_filter_multiple_sizes(
    service_client, authenticated_user, registered_org, dog_breed
):
    """Multiple values for the same filter param work (OR logic)"""
    small = await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        dog_breed['id'],
        {'size': 'small'},
    )
    large = await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        dog_breed['id'],
        {'size': 'large'},
    )
    medium = await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        dog_breed['id'],
        {'size': 'medium'},
    )

    response = await service_client.get('/animals?sizes=small&sizes=large')
    assert response.status == 200
    data = response.json()
    ids = [a['id'] for a in data]
    assert small['id'] in ids
    assert large['id'] in ids
    assert medium['id'] not in ids


async def test_animal_list_filter_breed_and_size(
    service_client, authenticated_user, registered_org, dog_breed, cat_breed
):
    """Combining breed and size filters narrows results correctly"""
    match = await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        dog_breed['id'],
        {'size': 'large'},
    )
    wrong_breed = await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        cat_breed['id'],
        {'size': 'large'},
    )
    wrong_size = await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        dog_breed['id'],
        {'size': 'small'},
    )

    response = await service_client.get(
        f'/animals?breeds={dog_breed["id"]}&sizes=large'
    )
    assert response.status == 200
    data = response.json()
    ids = [a['id'] for a in data]
    assert match['id'] in ids
    assert wrong_breed['id'] not in ids
    assert wrong_size['id'] not in ids


async def test_animal_list_filter_by_city(
    service_client, authenticated_user, dog_breed, pgsql
):
    conn = pgsql['postgres-db']
    cursor = conn.cursor()
    city1_name = f'City1_{uuid.uuid4().hex[:8]}'
    city2_name = f'City2_{uuid.uuid4().hex[:8]}'

    cursor.execute(
        'INSERT INTO cities (name) VALUES (%s) RETURNING id, name',
        (city1_name,),
    )
    city1 = {'id': cursor.fetchone()[0], 'name': city1_name}

    cursor.execute(
        'INSERT INTO cities (name) VALUES (%s) RETURNING id, name',
        (city2_name,),
    )
    city2 = {'id': cursor.fetchone()[0], 'name': city2_name}

    org1_response = await service_client.post(
        '/orgs',
        json={
            'name': f'Shelter_City1_{uuid.uuid4().hex[:8]}',
            'city_id': city1['id'],
        },
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )
    assert org1_response.status == 201
    org1 = org1_response.json()

    user2_email = f'owner2_{uuid.uuid4().hex[:8]}@example.com'
    user2_password = 'TestPassword123'

    await service_client.post(
        '/user/register',
        json={
            'email': user2_email,
            'first_name': 'Owner2',
            'last_name': 'User',
            'password': user2_password,
        },
    )

    login2_response = await service_client.post(
        '/auth/login',
        json={'email': user2_email, 'password': user2_password},
    )
    assert login2_response.status == 200
    user2_token = login2_response.json()['access_token']

    org2_response = await service_client.post(
        '/orgs',
        json={
            'name': f'Shelter_City2_{uuid.uuid4().hex[:8]}',
            'city_id': city2['id'],
        },
        headers={'Authorization': f"Bearer {user2_token}"},
    )
    assert org2_response.status == 201
    org2 = org2_response.json()

    animal1_response = await service_client.post(
        '/animals',
        json={
            'organization_id': org1['id'],
            'name': f'Dog_City1_{uuid.uuid4().hex[:8]}',
            'breed_id': dog_breed['id'],
            'size': 'medium',
            'gender': 'male',
            'care_level': 'easy',
            'good_with': 'dogs',
            'color': 'black',
            'age': 3,
            'status': 'available',
        },
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )
    assert animal1_response.status == 201

    animal2_response = await service_client.post(
        '/animals',
        json={
            'organization_id': org2['id'],
            'name': f'Dog_City2_{uuid.uuid4().hex[:8]}',
            'breed_id': dog_breed['id'],
            'size': 'large',
            'gender': 'female',
            'care_level': 'moderate',
            'good_with': 'cats',
            'color': 'white',
            'age': 5,
            'status': 'available',
        },
        headers={'Authorization': f"Bearer {user2_token}"},
    )
    assert animal2_response.status == 201

    list_response = await service_client.get(
        '/animals',
        params={'city_ids': str(city1['id'])},
    )
    assert list_response.status == 200
    animals = list_response.json()

    assert len(animals) == 1
    assert animals[0]['name'] == animal1_response.json()['name']

    list_response2 = await service_client.get(
        '/animals',
        params={'city_ids': str(city2['id'])},
    )
    assert list_response2.status == 200
    animals2 = list_response2.json()

    assert len(animals2) == 1
    assert animals2[0]['name'] == animal2_response.json()['name']


async def test_animal_list_filter_multiple_cities(
    service_client, authenticated_user, dog_breed, pgsql
):
    conn = pgsql['postgres-db']
    cursor = conn.cursor()

    city_names = [f'City_{uuid.uuid4().hex[:8]}' for _ in range(3)]
    cities = []

    for city_name in city_names:
        cursor.execute(
            'INSERT INTO cities (name) VALUES (%s) RETURNING id, name',
            (city_name,),
        )
        cities.append({'id': cursor.fetchone()[0], 'name': city_name})

    orgs = []
    tokens = [authenticated_user['token']]

    for i, city in enumerate(cities):
        if i == 0:
            token = authenticated_user['token']
        else:
            user_email = f'owner_multi_{i}_{uuid.uuid4().hex[:8]}@example.com'
            user_password = 'TestPassword123'

            await service_client.post(
                '/user/register',
                json={
                    'email': user_email,
                    'first_name': f'Owner{i}',
                    'last_name': 'User',
                    'password': user_password,
                },
            )

            login_response = await service_client.post(
                '/auth/login',
                json={'email': user_email, 'password': user_password},
            )
            assert login_response.status == 200
            token = login_response.json()['access_token']
            tokens.append(token)

        org_response = await service_client.post(
            '/orgs',
            json={
                'name': f'Shelter_{city["name"]}',
                'city_id': city['id'],
            },
            headers={'Authorization': f"Bearer {token}"},
        )
        assert org_response.status == 201
        orgs.append(org_response.json())

    animals = []
    for i, org in enumerate(orgs):
        animal_response = await service_client.post(
            '/animals',
            json={
                'organization_id': org['id'],
                'name': f'Dog_City{i}_{uuid.uuid4().hex[:8]}',
                'breed_id': dog_breed['id'],
                'size': 'small' if i == 0 else 'medium' if i == 1 else 'large',
                'gender': 'male' if i % 2 == 0 else 'female',
                'care_level': 'easy',
                'good_with': 'dogs',
                'color': 'black',
                'age': i + 1,
                'status': 'available',
            },
            headers={'Authorization': f"Bearer {tokens[i]}"},
        )
        assert animal_response.status == 201
        animals.append(animal_response.json())

    list_response = await service_client.get(
        '/animals',
        params={'city_ids': [str(cities[0]['id']), str(cities[1]['id'])]},
    )
    assert list_response.status == 200
    result = list_response.json()

    assert len(result) == 2
    result_names = {a['name'] for a in result}
    expected_names = {animals[0]['name'], animals[1]['name']}
    assert result_names == expected_names

    assert animals[2]['name'] not in result_names

    list_response2 = await service_client.get(
        '/animals',
        params={'city_ids': str(cities[2]['id'])},
    )
    assert list_response2.status == 200
    result2 = list_response2.json()

    assert len(result2) == 1
    assert result2[0]['name'] == animals[2]['name']

import uuid
import pytest


def make_unique_email(prefix='animal_delete'):
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
async def breed(pgsql):
    conn = pgsql['postgres-db']
    cursor = conn.cursor()
    breed_name = f'TestBreed_{uuid.uuid4().hex[:8]}'
    cursor.execute(
        'INSERT INTO breeds (name, animal_type) VALUES (%s, %s) RETURNING id, name',
        (breed_name, 'dog'),
    )
    row = cursor.fetchone()
    return {'id': row[0], 'name': row[1], 'animal_type': 'dog'}


@pytest.fixture
async def org_owner(service_client):
    email = make_unique_email('owner')
    password = 'TestPassword123'

    reg = await service_client.post(
        '/user/register',
        json={
            'email': email,
            'password': password,
            'first_name': 'Owner',
            'last_name': 'User',
        },
    )
    assert reg.status == 201

    login = await service_client.post(
        '/auth/login',
        json={'email': email, 'password': password},
    )
    assert login.status == 200
    token = login.json()['access_token']

    return {'token': token, 'email': email, 'password': password}


@pytest.fixture
async def org_with_owner(service_client, org_owner, city):
    response = await service_client.post(
        '/orgs',
        json={
            'name': 'Test Organization',
            'description': 'Test description',
            'city_id': city['id'],
        },
        headers={'Authorization': f"Bearer {org_owner['token']}"},
    )
    assert response.status == 201
    org = response.json()
    return {'org': org, 'token': org_owner['token'], 'city': city}


@pytest.fixture
async def animal(service_client, org_with_owner, breed):
    response = await service_client.post(
        '/animals',
        json={
            'organization_id': org_with_owner['org']['id'],
            'name': 'Test Animal',
            'breed_id': breed['id'],
            'size': 'medium',
            'gender': 'male',
            'care_level': 'easy',
            'good_with': 'children',
            'color': 'brown',
            'age': 3,
            'description': 'Test description',
            'status': 'available',
        },
        headers={'Authorization': f"Bearer {org_with_owner['token']}"},
    )
    assert response.status == 201
    animal_data = response.json()
    return {'id': animal_data['id'], 'name': 'Test Animal', 'token': org_with_owner['token']}


async def test_animal_delete_success(service_client, animal):
    animal_id = animal['id']
    token = animal['token']

    response = await service_client.delete(
        f'/animals/{animal_id}',
        headers={'Authorization': f'Bearer {token}'},
    )

    assert response.status == 204

    get_response = await service_client.get(f'/animals/{animal_id}')
    assert get_response.status == 404


async def test_animal_delete_requires_auth(service_client, animal):
    animal_id = animal['id']

    response = await service_client.delete(f'/animals/{animal_id}')

    assert response.status == 401


async def test_animal_delete_forbidden_for_non_owner(service_client, org_with_owner, breed):
    other_email = make_unique_email('another_owner')
    other_pass = 'TestPassword123'
    await service_client.post(
        '/user/register',
        json={
            'email': other_email,
            'password': other_pass,
            'first_name': 'Another',
            'last_name': 'Owner',
        },
    )
    login = await service_client.post(
        '/auth/login',
        json={'email': other_email, 'password': other_pass},
    )
    other_token = login.json()['access_token']

    another_org_response = await service_client.post(
        '/orgs',
        json={
            'name': 'Another Organization',
            'description': 'Another description',
            'city_id': org_with_owner['city']['id'],
        },
        headers={'Authorization': f'Bearer {other_token}'},
    )
    assert another_org_response.status == 201
    another_org = another_org_response.json()

    animal_response = await service_client.post(
        '/animals',
        json={
            'organization_id': another_org['id'],
            'name': 'Another Animal',
            'breed_id': breed['id'],
            'size': 'small',
            'gender': 'female',
            'care_level': 'easy',
            'good_with': 'children',
            'color': 'white',
            'age': 1,
            'description': 'Another description',
            'status': 'available',
        },
        headers={'Authorization': f'Bearer {other_token}'},
    )
    assert animal_response.status == 201
    another_animal = animal_response.json()

    response = await service_client.delete(
        f'/animals/{another_animal["id"]}',
        headers={'Authorization': f'Bearer {org_with_owner["token"]}'},
    )

    assert response.status == 403


async def test_animal_delete_nonexistent_animal(service_client, org_with_owner):
    token = org_with_owner['token']

    response = await service_client.delete(
        '/animals/999999999',
        headers={'Authorization': f'Bearer {token}'},
    )

    assert response.status == 404


async def test_animal_delete_invalid_id(service_client, org_with_owner):
    token = org_with_owner['token']

    response = await service_client.delete(
        '/animals/not-a-number',
        headers={'Authorization': f'Bearer {token}'},
    )

    assert response.status == 400


async def test_animal_delete_twice(service_client, animal):
    animal_id = animal['id']
    token = animal['token']

    response1 = await service_client.delete(
        f'/animals/{animal_id}',
        headers={'Authorization': f'Bearer {token}'},
    )
    assert response1.status == 204

    response2 = await service_client.delete(
        f'/animals/{animal_id}',
        headers={'Authorization': f'Bearer {token}'},
    )
    assert response2.status == 404

import pytest
import uuid


def make_unique_email(prefix='animalget'):
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
        'INSERT INTO breeds (animal_type, name) VALUES (%s, %s) RETURNING id, animal_type, name',
        ('dog', breed_name),
    )
    row = cursor.fetchone()
    return {'id': row[0], 'animal_type': row[1], 'name': row[2]}


@pytest.fixture
async def authenticated_user(service_client):
    email = make_unique_email()
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

    return {'token': tokens['access_token'], 'user_id': register_response.json()['id']}


@pytest.fixture
async def registered_org(service_client, authenticated_user, city):
    response = await service_client.post(
        '/orgs',
        json={
            'name': 'Happy Paws Shelter',
            'description': 'A shelter for happy paws',
            'city_id': city['id'],
        },
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )
    assert response.status == 201
    return response.json()


@pytest.fixture
async def registered_animal(service_client, authenticated_user, registered_org, breed):
    response = await service_client.post(
        '/animals',
        json={
            'name': 'Buddy',
            'size': 'large',
            'gender': 'male',
            'care_level': 'easy',
            'color': 'brown',
            'good_with': 'children',
            'age': 3,
            'status': 'available',
            'organization_id': registered_org['id'],
            'breed_id': breed['id'],
        },
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )
    assert response.status == 201
    return response.json()


async def test_get_animal_success(service_client, registered_animal):
    animal_id = registered_animal['id']
    response = await service_client.get(f'/animals/{animal_id}')
    assert response.status == 200
    data = response.json()
    assert data['id'] == animal_id
    assert data['name'] == 'Buddy'
    assert data['size'] == 'large'
    assert data['gender'] == 'male'
    assert data['care_level'] == 'easy'
    assert data['color'] == 'brown'
    assert data['good_with'] == 'children'
    assert data['age'] == 3
    assert data['status'] == 'available'
    assert data['organization_id'] == registered_animal['organization_id']
    assert data['breed']['id'] == registered_animal['breed']['id']
    assert data['can_be_adopted'] is False


async def test_get_animal_can_be_adopted_true_for_authenticated_user(
    service_client, authenticated_user, registered_animal, pgsql
):
    conn = pgsql['postgres-db']
    cursor = conn.cursor()
    cursor.execute(
        'DELETE FROM adopt_requests WHERE animal_id = %s AND user_id = %s',
        (registered_animal['id'], authenticated_user['user_id']),
    )
    animal_id = registered_animal['id']
    response = await service_client.get(
        f'/animals/{animal_id}',
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )
    assert response.status == 200
    assert response.json()['can_be_adopted'] is True


async def test_get_animal_can_be_adopted_false_when_request_exists(
    service_client, authenticated_user, registered_animal, pgsql
):
    conn = pgsql['postgres-db']
    cursor = conn.cursor()
    cursor.execute(
        'INSERT INTO adopt_requests (animal_id, user_id) VALUES (%s, %s)',
        (registered_animal['id'], authenticated_user['user_id']),
    )

    response = await service_client.get(
        f"/animals/{registered_animal['id']}",
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )
    assert response.status == 200
    assert response.json()['can_be_adopted'] is False


async def test_get_animal_not_found(service_client):
    response = await service_client.get('/animals/9999')
    assert response.status == 404
    data = response.json()
    assert data['error']['code'] == 'ANIMAL_NOT_FOUND'


async def test_get_animal_invalid_id(service_client):
    response = await service_client.get('/animals/invalid_id')
    assert response.status == 404
    data = response.json()
    assert data['error']['code'] == 'ANIMAL_NOT_FOUND'

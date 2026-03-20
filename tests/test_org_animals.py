import pytest
import uuid

VALID_ANIMAL_PAYLOADS = [
    {
        'name': 'Buddy',
        'size': 'large',
        'gender': 'male',
        'care_level': 'easy',
        'color': 'brown',
        'good_with': 'children',
        'age': 3,
        'status': 'available',
    },
    {
        'name': 'Molly',
        'size': 'small',
        'gender': 'female',
        'care_level': 'easy',
        'color': 'brown',
        'good_with': 'dogs',
        'age': 2,
        'status': 'available',
    },
]


def make_unique_email(prefix='animal'):
    random_part = uuid.uuid4().hex[:10]
    return f'{prefix}_{random_part}@example.com'


@pytest.fixture
async def authenticated_user(service_client):
    email = make_unique_email('org_animal')
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
async def registered_animals(service_client, authenticated_user, registered_org, breed):
    animals = list()
    for payload in VALID_ANIMAL_PAYLOADS:
        data = payload.copy()
        data.update({
            'organization_id': registered_org['id'],
            'breed_id': breed['id'],
        })
        response = await service_client.post(
            '/animals',
            json=data,
            headers={'Authorization': f"Bearer {authenticated_user['token']}"},
        )
        assert response.status == 201
        animals.append(response.json())
    return animals


async def test_org_animals_success(service_client, registered_animals, registered_org):
    """Test successfully getting animals of an organization"""
    org_id = registered_org['id']

    response = await service_client.get(f'/orgs/{org_id}/animals')
    assert response.status == 200
    animals = response.json()
    assert isinstance(animals, list)
    returned_names = {animal['name'] for animal in animals}
    for payload in VALID_ANIMAL_PAYLOADS:
        assert payload['name'] in returned_names


async def test_org_animals_empty(service_client, registered_org):
    """Test getting animals for organization with no animals"""
    org_id = registered_org['id']
    response = await service_client.get(f'/orgs/{org_id}/animals')
    assert response.status == 200
    animals = response.json()
    assert isinstance(animals, list)
    assert len(animals) == 0


async def test_org_animals_not_found(service_client):
    """Test getting animals for non-existent organization"""
    response = await service_client.get('/orgs/999999/animals')
    assert response.status == 404
    data = response.json()
    assert data['error']['code'] == 'ORGANIZATION_NOT_FOUND'


async def test_org_animals_invalid_id(service_client):
    """Test getting animals with invalid organization ID format"""
    response = await service_client.get('/orgs/invalid_id/animals')
    assert response.status == 404
    data = response.json()
    assert data['error']['code'] == 'ORGANIZATION_NOT_FOUND'

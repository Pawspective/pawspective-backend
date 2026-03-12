import uuid
import pytest


def make_unique_email(prefix='get_org'):
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
            'name': 'Happy Paws Shelter',
            'description': 'A shelter for happy paws',
            'city_id': city['id'],
        },
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )
    assert response.status == 201
    return response.json()


async def test_get_org_success(service_client, registered_org, city):
    """Test successfully getting an organization by ID"""
    org_id = registered_org['id']

    response = await service_client.get(f'/orgs/{org_id}')

    assert response.status == 200
    data = response.json()
    assert data['id'] == org_id
    assert data['name'] == 'Happy Paws Shelter'
    assert data['description'] == 'A shelter for happy paws'
    assert data['city']['id'] == city['id']
    assert data['city']['name'] == city['name']


async def test_get_org_without_description(service_client, authenticated_user, city):
    """Test getting an organization that has no description"""
    create_response = await service_client.post(
        '/orgs',
        json={
            'name': 'No Desc Org',
            'city_id': city['id'],
        },
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )
    assert create_response.status == 201
    org_id = create_response.json()['id']

    response = await service_client.get(f'/orgs/{org_id}')

    assert response.status == 200
    data = response.json()
    assert data['id'] == org_id
    assert data['name'] == 'No Desc Org'
    assert data['description'] is None


async def test_get_org_not_found(service_client):
    """Test getting an organization that does not exist returns 404"""
    response = await service_client.get('/orgs/999999999')

    assert response.status == 404
    data = response.json()
    assert data['error']['code'] == 'ORGANIZATION_NOT_FOUND'


async def test_get_org_invalid_id_format(service_client):
    """Test getting an organization with a non-numeric ID returns 404"""
    response = await service_client.get('/orgs/not-a-number')

    assert response.status == 404
    data = response.json()
    assert data['error']['code'] == 'ORGANIZATION_NOT_FOUND'

import uuid
import pytest


def make_unique_email(prefix='org'):
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
    """Fixture to create and login a user for testing"""
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


async def test_register_org_success(service_client, authenticated_user, city):
    """Test successful organization registration"""
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
    data = response.json()
    assert data['name'] == 'Happy Paws Shelter'
    assert data['description'] == 'A shelter for happy paws'
    assert 'id' in data
    assert data['city']['id'] == city['id']


async def test_register_org_success_without_description(service_client, authenticated_user, city):
    """Test successful registration without optional description"""
    response = await service_client.post(
        '/orgs',
        json={
            'name': 'City Animal Rescue',
            'city_id': city['id'],
        },
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )

    assert response.status == 201
    data = response.json()
    assert data['name'] == 'City Animal Rescue'
    assert data['city']['id'] == city['id']
    assert 'id' in data


async def test_register_org_requires_auth(service_client, city):
    """Test that creating an organization requires authentication"""
    response = await service_client.post(
        '/orgs',
        json={
            'name': 'Unauthorized Org',
            'city_id': city['id'],
        },
    )

    assert response.status == 401


async def test_register_org_missing_name(service_client, authenticated_user, city):
    """Test missing required 'name' field"""
    response = await service_client.post(
        '/orgs',
        json={
            'city_id': city['id'],
        },
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )

    assert response.status == 400
    payload = response.json()
    assert payload['error']['code'] == 'MISSING_FIELD'
    assert payload['error']['message'] == 'Missing required field'


async def test_register_org_missing_city_id(service_client, authenticated_user):
    """Test missing required 'city_id' field"""
    response = await service_client.post(
        '/orgs',
        json={
            'name': 'No City Org',
        },
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )

    assert response.status == 400
    payload = response.json()
    assert payload['error']['code'] == 'MISSING_FIELD'
    assert payload['error']['message'] == 'Missing required field'


async def test_register_org_blank_name(service_client, authenticated_user, city):
    """Test blank name fails validation"""
    response = await service_client.post(
        '/orgs',
        json={
            'name': '   ',
            'city_id': city['id'],
        },
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )

    assert response.status == 400
    payload = response.json()
    assert payload['error']['code'] == 'VALIDATION_ERROR'
    assert payload['error']['message'] == 'Validation failed'


async def test_register_org_invalid_json(service_client, authenticated_user):
    """Test invalid JSON body returns 400"""
    response = await service_client.post(
        '/orgs',
        data='not valid json',
        headers={
            'Content-Type': 'application/json',
            'Authorization': f"Bearer {authenticated_user['token']}",
        },
    )

    assert response.status == 400

import uuid
import pytest


def make_unique_email(prefix='animal'):
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


VALID_ANIMAL_PAYLOAD = {
    'name': 'Buddy',
    'size': 'large',
    'gender': 'male',
    'care_level': 'easy',
    'color': 'brown',
    'good_with': 'children',
    'age': 3,
    'status': 'available',
}


async def test_register_animal_success(
    service_client, authenticated_user, registered_org, breed
):
    """Test successful animal registration"""
    response = await service_client.post(
        '/animals',
        json={
            **VALID_ANIMAL_PAYLOAD,
            'organization_id': registered_org['id'],
            'breed_id': breed['id'],
        },
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )

    assert response.status == 201
    data = response.json()
    assert data['name'] == 'Buddy'
    assert data['organization_id'] == registered_org['id']
    assert data['breed']['id'] == breed['id']
    assert data['size'] == 'large'
    assert data['gender'] == 'male'
    assert data['care_level'] == 'easy'
    assert data['color'] == 'brown'
    assert data['good_with'] == 'children'
    assert data['age'] == 3
    assert data['status'] == 'available'
    assert 'id' in data


async def test_register_animal_with_description(
    service_client, authenticated_user, registered_org, breed
):
    """Test successful animal registration with optional description"""
    response = await service_client.post(
        '/animals',
        json={
            **VALID_ANIMAL_PAYLOAD,
            'organization_id': registered_org['id'],
            'breed_id': breed['id'],
            'description': 'A very friendly dog',
        },
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )

    assert response.status == 201
    data = response.json()
    assert data['description'] == 'A very friendly dog'


async def test_register_animal_requires_auth(service_client, registered_org, breed):
    """Test that registering an animal requires authentication"""
    response = await service_client.post(
        '/animals',
        json={
            **VALID_ANIMAL_PAYLOAD,
            'organization_id': registered_org['id'],
            'breed_id': breed['id'],
        },
    )

    assert response.status == 401


async def test_register_animal_forbidden_no_org(
    service_client, registered_org, breed
):
    """Test that a user without an organization cannot register an animal"""
    email = make_unique_email('noorg')
    password = 'TestPassword123'

    await service_client.post(
        '/user/register',
        json={
            'email': email,
            'password': password,
            'first_name': 'No',
            'last_name': 'Org',
        },
    )
    login_response = await service_client.post(
        '/auth/login',
        json={'email': email, 'password': password},
    )
    token = login_response.json()['access_token']

    response = await service_client.post(
        '/animals',
        json={
            **VALID_ANIMAL_PAYLOAD,
            'organization_id': registered_org['id'],
            'breed_id': breed['id'],
        },
        headers={'Authorization': f'Bearer {token}'},
    )

    assert response.status == 403
    data = response.json()
    assert data['code'] == '403'


async def test_register_animal_forbidden_wrong_org(
    service_client, authenticated_user, city, breed
):
    """Test that a user cannot register an animal for another organization"""
    other_email = make_unique_email('other')
    password = 'TestPassword123'

    await service_client.post(
        '/user/register',
        json={
            'email': other_email,
            'password': password,
            'first_name': 'Other',
            'last_name': 'Owner',
        },
    )
    other_login = await service_client.post(
        '/auth/login',
        json={'email': other_email, 'password': password},
    )
    other_token = other_login.json()['access_token']

    other_org_response = await service_client.post(
        '/orgs',
        json={'name': 'Other Shelter', 'city_id': city['id']},
        headers={'Authorization': f'Bearer {other_token}'},
    )
    assert other_org_response.status == 201
    other_org_id = other_org_response.json()['id']

    response = await service_client.post(
        '/animals',
        json={
            **VALID_ANIMAL_PAYLOAD,
            'organization_id': other_org_id,
            'breed_id': breed['id'],
        },
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )

    assert response.status == 403
    data = response.json()
    assert data['code'] == '403'


async def test_register_animal_breed_not_found(
    service_client, authenticated_user, registered_org
):
    """Test that a non-existent breed_id returns 404"""
    response = await service_client.post(
        '/animals',
        json={
            **VALID_ANIMAL_PAYLOAD,
            'organization_id': registered_org['id'],
            'breed_id': 999999999,
        },
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )

    assert response.status == 404
    data = response.json()
    assert data['error']['code'] == 'BREED_NOT_FOUND'


async def test_register_animal_missing_name(
    service_client, authenticated_user, registered_org, breed
):
    """Test missing required 'name' field"""
    payload = {
        **VALID_ANIMAL_PAYLOAD,
        'organization_id': registered_org['id'],
        'breed_id': breed['id'],
    }
    del payload['name']

    response = await service_client.post(
        '/animals',
        json=payload,
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )

    assert response.status == 400
    data = response.json()
    assert data['error']['code'] == 'MISSING_FIELD'


async def test_register_animal_missing_breed_id(
    service_client, authenticated_user, registered_org
):
    """Test missing required 'breed_id' field"""
    payload = {
        **VALID_ANIMAL_PAYLOAD,
        'organization_id': registered_org['id'],
    }

    response = await service_client.post(
        '/animals',
        json=payload,
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )

    assert response.status == 400
    data = response.json()
    assert data['error']['code'] == 'MISSING_FIELD'


async def test_register_animal_blank_name(
    service_client, authenticated_user, registered_org, breed
):
    """Test blank name fails validation"""
    response = await service_client.post(
        '/animals',
        json={
            **VALID_ANIMAL_PAYLOAD,
            'organization_id': registered_org['id'],
            'breed_id': breed['id'],
            'name': '   ',
        },
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )

    assert response.status == 400
    data = response.json()
    assert data['error']['code'] == 'VALIDATION_ERROR'


async def test_register_animal_invalid_json(service_client, authenticated_user):
    """Test invalid JSON body returns 400"""
    response = await service_client.post(
        '/animals',
        data='not valid json',
        headers={
            'Content-Type': 'application/json',
            'Authorization': f"Bearer {authenticated_user['token']}",
        },
    )

    assert response.status == 400

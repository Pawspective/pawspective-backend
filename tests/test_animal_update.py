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


async def test_update_animal_success(
    service_client, authenticated_user, registered_animal
):
    """Test successful animal update"""
    response = await service_client.put(
        f"/animals/{registered_animal['id']}",
        json={'name': 'Max', 'age': 4, 'status': 'unavailable'},
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )

    assert response.status == 200
    data = response.json()
    assert data['id'] == registered_animal['id']
    assert data['name'] == 'Max'
    assert data['age'] == 4
    assert data['status'] == 'unavailable'
    assert data['size'] == 'large'
    assert data['gender'] == 'male'


async def test_update_animal_partial_fields(
    service_client, authenticated_user, registered_animal
):
    """Test that only provided fields are updated"""
    response = await service_client.put(
        f"/animals/{registered_animal['id']}",
        json={'color': 'black', 'good_with': 'dogs'},
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )

    assert response.status == 200
    data = response.json()
    assert data['color'] == 'black'
    assert data['good_with'] == 'dogs'
    assert data['name'] == registered_animal['name']


async def test_update_animal_requires_auth(service_client, registered_animal):
    """Test that updating an animal requires authentication"""
    response = await service_client.put(
        f"/animals/{registered_animal['id']}",
        json={'name': 'Max'},
    )

    assert response.status == 401


async def test_update_animal_not_found(service_client, authenticated_user):
    """Test that updating a non-existent animal returns 404"""
    response = await service_client.put(
        '/animals/999999999',
        json={'name': 'Max'},
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )

    assert response.status == 404
    data = response.json()
    assert data['error']['code'] == 'ANIMAL_NOT_FOUND'


async def test_update_animal_forbidden_wrong_org(
    service_client, registered_animal, city, breed
):
    """Test that a user cannot update an animal belonging to another organization"""
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

    await service_client.post(
        '/orgs',
        json={'name': 'Other Shelter', 'city_id': city['id']},
        headers={'Authorization': f'Bearer {other_token}'},
    )

    response = await service_client.put(
        f"/animals/{registered_animal['id']}",
        json={'name': 'Stolen'},
        headers={'Authorization': f'Bearer {other_token}'},
    )

    assert response.status == 403
    data = response.json()
    assert data['error']['code'] == 'FORBIDDEN'


async def test_update_animal_blank_name(
    service_client, authenticated_user, registered_animal
):
    """Test that setting a blank name fails validation"""
    response = await service_client.put(
        f"/animals/{registered_animal['id']}",
        json={'name': '   '},
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )

    assert response.status == 400
    data = response.json()
    assert data['error']['code'] == 'VALIDATION_ERROR'


async def test_update_animal_status_adopted_forbidden(
    service_client, authenticated_user, registered_animal
):
    """Test that setting status to adopted via update is forbidden"""
    response = await service_client.put(
        f"/animals/{registered_animal['id']}",
        json={'status': 'adopted'},
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )

    assert response.status == 403
    assert response.json()['error']['code'] == 'FORBIDDEN'


async def test_update_animal_invalid_id(service_client, authenticated_user):
    """Test that a non-numeric animal ID returns 404"""
    response = await service_client.put(
        '/animals/not-a-number',
        json={'name': 'Max'},
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )

    assert response.status == 404


async def test_update_status_to_available_clears_user_id(
    service_client, authenticated_user, registered_animal, pgsql
):
    """Test that changing status to available clears user_id"""
    conn = pgsql['postgres-db']
    cursor = conn.cursor()
    cursor.execute(
        'UPDATE animals SET user_id = (SELECT id FROM users LIMIT 1), status = %s WHERE id = %s',
        ('adopted', registered_animal['id']),
    )

    response = await service_client.put(
        f"/animals/{registered_animal['id']}",
        json={'status': 'available'},
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )

    assert response.status == 200
    assert response.json()['status'] == 'available'

    cursor.execute('SELECT user_id FROM animals WHERE id = %s',
                   (registered_animal['id'],))
    row = cursor.fetchone()
    assert row[0] is None


async def test_update_status_to_unavailable_clears_user_id(
    service_client, authenticated_user, registered_animal, pgsql
):
    """Test that changing status to unavailable clears user_id"""
    conn = pgsql['postgres-db']
    cursor = conn.cursor()
    cursor.execute(
        'UPDATE animals SET user_id = (SELECT id FROM users LIMIT 1), status = %s WHERE id = %s',
        ('adopted', registered_animal['id']),
    )

    response = await service_client.put(
        f"/animals/{registered_animal['id']}",
        json={'status': 'unavailable'},
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )

    assert response.status == 200
    assert response.json()['status'] == 'unavailable'

    cursor.execute('SELECT user_id FROM animals WHERE id = %s',
                   (registered_animal['id'],))
    row = cursor.fetchone()
    assert row[0] is None

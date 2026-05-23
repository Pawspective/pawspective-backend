import uuid

import pytest


def make_unique_email(prefix='adopt_user'):
    return f'{prefix}_{uuid.uuid4().hex[:10]}@example.com'


@pytest.fixture
async def city(pgsql):
    conn = pgsql['postgres-db']
    cursor = conn.cursor()
    cursor.execute(
        'INSERT INTO cities (name) VALUES (%s) RETURNING id',
        (f'City_{uuid.uuid4().hex[:8]}',),
    )
    return {'id': cursor.fetchone()[0]}


@pytest.fixture
async def breed(pgsql):
    conn = pgsql['postgres-db']
    cursor = conn.cursor()
    cursor.execute(
        "INSERT INTO breeds (animal_type, name) VALUES ('dog', %s) RETURNING id",
        (f'Breed_{uuid.uuid4().hex[:8]}',),
    )
    return {'id': cursor.fetchone()[0]}


@pytest.fixture
async def org_owner(service_client, city):
    email = make_unique_email('owner')
    password = 'TestPassword123'

    reg = await service_client.post(
        '/user/register',
        json={'email': email, 'password': password,
              'first_name': 'Owner', 'last_name': 'User'},
    )
    assert reg.status == 201

    login = await service_client.post('/auth/login', json={'email': email, 'password': password})
    assert login.status == 200
    token = login.json()['access_token']

    org = await service_client.post(
        '/orgs',
        json={
            'name': f'Shelter_{uuid.uuid4().hex[:8]}', 'city_id': city['id']},
        headers={'Authorization': f'Bearer {token}'},
    )
    assert org.status == 201
    return {'token': token, 'org_id': org.json()['id']}


@pytest.fixture
async def adopter(service_client):
    email = make_unique_email('adopter')
    password = 'TestPassword123'

    reg = await service_client.post(
        '/user/register',
        json={'email': email, 'password': password,
              'first_name': 'Adopter', 'last_name': 'User'},
    )
    assert reg.status == 201

    login = await service_client.post('/auth/login', json={'email': email, 'password': password})
    assert login.status == 200
    return {'token': login.json()['access_token'], 'email': email}


@pytest.fixture
async def available_animal(pgsql, org_owner, breed):
    conn = pgsql['postgres-db']
    cursor = conn.cursor()
    cursor.execute(
        """
        INSERT INTO animals
            (organization_id, name, breed_id, size, gender, care_level, good_with, color, age, status)
        VALUES (%s, %s, %s, 'medium', 'male', 'easy', 'dogs', 'brown', 2, 'available')
        RETURNING id
        """,
        (org_owner['org_id'], f'Animal_{uuid.uuid4().hex[:6]}', breed['id']),
    )
    return {'id': cursor.fetchone()[0]}


@pytest.fixture
async def unavailable_animal(pgsql, org_owner, breed):
    conn = pgsql['postgres-db']
    cursor = conn.cursor()
    cursor.execute(
        """
        INSERT INTO animals
            (organization_id, name, breed_id, size, gender, care_level, good_with, color, age, status)
        VALUES (%s, %s, %s, 'large', 'female', 'moderate', 'cats', 'black', 4, 'adopted')
        RETURNING id
        """,
        (org_owner['org_id'], f'Animal_{uuid.uuid4().hex[:6]}', breed['id']),
    )
    return {'id': cursor.fetchone()[0]}


async def test_adopt_returns_201(service_client, adopter, available_animal):
    """Authenticated user can create an adopt request"""
    response = await service_client.post(
        f'/animals/{available_animal["id"]}/adopt',
        headers={'Authorization': f'Bearer {adopter["token"]}'},
    )
    assert response.status == 201


async def test_adopt_response_shape(service_client, adopter, available_animal):
    """Response contains id, email, and animal fields"""
    response = await service_client.post(
        f'/animals/{available_animal["id"]}/adopt',
        headers={'Authorization': f'Bearer {adopter["token"]}'},
    )
    assert response.status == 201
    data = response.json()
    assert 'id' in data
    assert 'email' in data
    assert 'animal' in data
    assert data['email'] == adopter['email']
    assert data['animal']['id'] == available_animal['id']


async def test_adopt_creates_db_record(service_client, pgsql, adopter, available_animal):
    """Adopt request appears in the database after creation"""
    response = await service_client.post(
        f'/animals/{available_animal["id"]}/adopt',
        headers={'Authorization': f'Bearer {adopter["token"]}'},
    )
    assert response.status == 201
    request_id = response.json()['id']

    cursor = pgsql['postgres-db'].cursor()
    cursor.execute(
        'SELECT id FROM adopt_requests WHERE id = %s', (request_id,))
    assert cursor.fetchone() is not None


async def test_adopt_duplicate_returns_409(service_client, adopter, available_animal):
    """Second request from the same user for the same animal returns 409"""
    await service_client.post(
        f'/animals/{available_animal["id"]}/adopt',
        headers={'Authorization': f'Bearer {adopter["token"]}'},
    )
    response = await service_client.post(
        f'/animals/{available_animal["id"]}/adopt',
        headers={'Authorization': f'Bearer {adopter["token"]}'},
    )
    assert response.status == 409
    assert response.json()['error']['code'] == 'ADOPT_REQUEST_ALREADY_EXISTS'


async def test_adopt_unavailable_animal_returns_409(service_client, adopter, unavailable_animal):
    """Request for an already-adopted animal returns 409"""
    response = await service_client.post(
        f'/animals/{unavailable_animal["id"]}/adopt',
        headers={'Authorization': f'Bearer {adopter["token"]}'},
    )
    assert response.status == 409
    assert response.json()['error']['code'] == 'ANIMAL_NOT_AVAILABLE'


async def test_adopt_nonexistent_animal_returns_404(service_client, adopter):
    """Non-existent animal ID returns 404"""
    response = await service_client.post(
        '/animals/999999999/adopt',
        headers={'Authorization': f'Bearer {adopter["token"]}'},
    )
    assert response.status == 404
    assert response.json()['error']['code'] == 'ANIMAL_NOT_FOUND'


async def test_adopt_without_auth_returns_401(service_client, available_animal):
    """Request without auth token returns 401"""
    response = await service_client.post(f'/animals/{available_animal["id"]}/adopt')
    assert response.status == 401


async def test_adopt_invalid_id_returns_400(service_client, adopter):
    """Non-integer animal ID returns 400"""
    response = await service_client.post(
        '/animals/not-an-id/adopt',
        headers={'Authorization': f'Bearer {adopter["token"]}'},
    )
    assert response.status == 400


async def test_adopt_different_users_same_animal(service_client, available_animal):
    """Two different users can both submit adopt requests for the same animal"""
    user1_email = make_unique_email('user1')
    user2_email = make_unique_email('user2')
    password = 'TestPassword123'

    for email in (user1_email, user2_email):
        await service_client.post(
            '/user/register',
            json={'email': email, 'password': password,
                  'first_name': 'U', 'last_name': 'U'},
        )

    token1 = (await service_client.post('/auth/login', json={'email': user1_email, 'password': password})).json()['access_token']
    token2 = (await service_client.post('/auth/login', json={'email': user2_email, 'password': password})).json()['access_token']

    r1 = await service_client.post(
        f'/animals/{available_animal["id"]}/adopt',
        headers={'Authorization': f'Bearer {token1}'},
    )
    r2 = await service_client.post(
        f'/animals/{available_animal["id"]}/adopt',
        headers={'Authorization': f'Bearer {token2}'},
    )
    assert r1.status == 201
    assert r2.status == 201
    assert r1.json()['id'] != r2.json()['id']

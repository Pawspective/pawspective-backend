import uuid

import pytest


def make_unique_email(prefix='accept_user'):
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
        "INSERT INTO breeds (animal_type, name) VALUES ('cat', %s) RETURNING id",
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
    owner_id = reg.json()['id']

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
    return {'token': token, 'org_id': org.json()['id'], 'user_id': owner_id}


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
    adopter_id = reg.json()['id']

    login = await service_client.post('/auth/login', json={'email': email, 'password': password})
    assert login.status == 200
    return {'token': login.json()['access_token'], 'user_id': adopter_id}


@pytest.fixture
async def animal(pgsql, org_owner, breed):
    conn = pgsql['postgres-db']
    cursor = conn.cursor()
    cursor.execute(
        """
        INSERT INTO animals
            (organization_id, name, breed_id, size, gender, care_level, good_with, color, age, status)
        VALUES (%s, %s, %s, 'small', 'male', 'easy', 'dogs', 'black', 2, 'available')
        RETURNING id
        """,
        (org_owner['org_id'], f'Animal_{uuid.uuid4().hex[:6]}', breed['id']),
    )
    return {'id': cursor.fetchone()[0]}


@pytest.fixture
async def adopt_request(pgsql, animal, adopter):
    conn = pgsql['postgres-db']
    cursor = conn.cursor()
    cursor.execute(
        'INSERT INTO adopt_requests (animal_id, user_id) VALUES (%s, %s) RETURNING id',
        (animal['id'], adopter['user_id']),
    )
    return {'id': cursor.fetchone()[0]}


async def test_accept_returns_204(service_client, org_owner, adopt_request):
    """Org owner can accept an adopt request"""
    response = await service_client.post(
        f'/requests/accept/{adopt_request["id"]}',
        headers={'Authorization': f'Bearer {org_owner["token"]}'},
    )
    assert response.status == 204


async def test_accept_removes_request(service_client, pgsql, org_owner, adopt_request):
    """After accept, the adopt request is deleted from the database"""
    await service_client.post(
        f'/requests/accept/{adopt_request["id"]}',
        headers={'Authorization': f'Bearer {org_owner["token"]}'},
    )
    cursor = pgsql['postgres-db'].cursor()
    cursor.execute('SELECT id FROM adopt_requests WHERE id = %s',
                   (adopt_request['id'],))
    assert cursor.fetchone() is None


async def test_accept_marks_animal_adopted(service_client, pgsql, org_owner, adopt_request, animal):
    """After accept, the animal status becomes 'adopted'"""
    await service_client.post(
        f'/requests/accept/{adopt_request["id"]}',
        headers={'Authorization': f'Bearer {org_owner["token"]}'},
    )
    cursor = pgsql['postgres-db'].cursor()
    cursor.execute('SELECT status FROM animals WHERE id = %s', (animal['id'],))
    assert cursor.fetchone()[0] == 'adopted'


async def test_accept_without_auth_returns_401(service_client, adopt_request):
    """Request without auth token returns 401"""
    response = await service_client.post(f'/requests/accept/{adopt_request["id"]}')
    assert response.status == 401


async def test_accept_non_owner_returns_403(service_client, city, adopt_request):
    """User not belonging to the animal's org gets 403"""
    email = make_unique_email('stranger')
    password = 'TestPassword123'
    await service_client.post(
        '/user/register',
        json={'email': email, 'password': password,
              'first_name': 'S', 'last_name': 'S'},
    )
    login = await service_client.post('/auth/login', json={'email': email, 'password': password})
    stranger_token = login.json()['access_token']

    response = await service_client.post(
        f'/requests/accept/{adopt_request["id"]}',
        headers={'Authorization': f'Bearer {stranger_token}'},
    )
    assert response.status == 403


async def test_accept_other_org_owner_returns_403(service_client, city, adopt_request):
    """Owner of a different org cannot accept the request"""
    email = make_unique_email('other_owner')
    password = 'TestPassword123'
    await service_client.post(
        '/user/register',
        json={'email': email, 'password': password,
              'first_name': 'O', 'last_name': 'O'},
    )
    login = await service_client.post('/auth/login', json={'email': email, 'password': password})
    token = login.json()['access_token']
    await service_client.post(
        '/orgs',
        json={
            'name': f'OtherOrg_{uuid.uuid4().hex[:8]}', 'city_id': city['id']},
        headers={'Authorization': f'Bearer {token}'},
    )

    response = await service_client.post(
        f'/requests/accept/{adopt_request["id"]}',
        headers={'Authorization': f'Bearer {token}'},
    )
    assert response.status == 403


async def test_accept_nonexistent_request_returns_404(service_client, org_owner):
    """Non-existent request ID returns 404"""
    response = await service_client.post(
        '/requests/accept/999999999',
        headers={'Authorization': f'Bearer {org_owner["token"]}'},
    )
    assert response.status == 404


async def test_accept_invalid_id_returns_400(service_client, org_owner):
    """Non-integer request ID returns 400"""
    response = await service_client.post(
        '/requests/accept/not-an-id',
        headers={'Authorization': f'Bearer {org_owner["token"]}'},
    )
    assert response.status == 400

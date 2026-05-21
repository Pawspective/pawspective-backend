import uuid

import pytest

def make_unique_email(prefix='review_create'):
    random_part = uuid.uuid4().hex[:10]
    return f'{prefix}_{random_part}@example.com'


@pytest.fixture
async def city(pgsql):
    conn = pgsql['postgres-db']
    cursor = conn.cursor()
    city_name = f'review_create_{uuid.uuid4().hex[:8]}'
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
    breed_name = f'review_create_{uuid.uuid4().hex[:8]}'
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
            'name': f'Shelter_{uuid.uuid4().hex[:8]}',
            'city_id': city['id'],
        },
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )
    assert response.status == 201
    return response.json()


@pytest.fixture
async def registered_animal(service_client, authenticated_user, registered_org, breed):
    payload = {
        'name': 'Buddy',
        'size': 'medium',
        'gender': 'male',
        'care_level': 'easy',
        'color': 'brown',
        'good_with': 'children',
        'age': 2,
        'status': 'available',
        'organization_id': registered_org['id'],
        'breed_id': breed['id'],
    }
    response = await service_client.post(
        '/animals',
        json=payload,
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )
    assert response.status == 201
    return response.json()


async def test_create_review_success(service_client, authenticated_user, registered_animal):
    """POST /reviews returns 201 and a ReviewDTO"""
    text = f'Review text {uuid.uuid4().hex[:6]}'

    response = await service_client.post(
        '/reviews',
        json={'animal_id': registered_animal['id'], 'text': text},
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )

    assert response.status == 201
    data = response.json()
    assert 'id' in data
    assert data['animal_id'] == registered_animal['id']
    assert data['text'] == text
    assert 'animal_name' in data
    assert 'created_at' in data
    assert data.get('can_edit') is True


async def test_create_review_no_duplicates(service_client, authenticated_user, registered_animal, pgsql):
    text = 'Initial review'
    r1 = await service_client.post(
        '/reviews',
        json={'animal_id': registered_animal['id'], 'text': text},
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )
    assert r1.status == 201
    first_id = r1.json()['id']

    r2 = await service_client.post(
        '/reviews',
        json={'animal_id': registered_animal['id'], 'text': 'Second review'},
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )
    assert r2.status == 409
    assert r2.json()['error']['code'] == 'REVIEW_ALREADY_EXISTS'


async def test_create_review_missing_fields_returns_400(service_client, authenticated_user):
    response = await service_client.post(
        '/reviews',
        json={'text': 'No animal id'},
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )
    assert response.status == 400
    payload = response.json()
    assert payload['error']['code'] == 'MISSING_FIELD'


async def test_create_review_text_too_long_returns_400(service_client, authenticated_user, registered_animal):
    long_text = 'a' * 2001
    response = await service_client.post(
        '/reviews',
        json={'animal_id': registered_animal['id'], 'text': long_text},
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )
    assert response.status == 400
    payload = response.json()
    assert payload['error']['code'] == 'VALIDATION_ERROR'


async def test_create_review_unauthenticated_returns_401(service_client, registered_animal):
    response = await service_client.post(
        '/reviews',
        json={'animal_id': registered_animal['id'], 'text': 'No auth'},
    )
    assert response.status == 401

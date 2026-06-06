import uuid

import pytest


def make_unique_email(prefix='review_update'):
    random_part = uuid.uuid4().hex[:10]
    return f'{prefix}_{random_part}@example.com'


@pytest.fixture
async def city(pgsql):
    conn = pgsql['postgres-db']
    cursor = conn.cursor()
    city_name = f'review_update_{uuid.uuid4().hex[:8]}'
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
    breed_name = f'review_update_{uuid.uuid4().hex[:8]}'
    cursor.execute(
        'INSERT INTO breeds (animal_type, name) VALUES (%s, %s) RETURNING id, animal_type, name',
        ('dog', breed_name),
    )
    row = cursor.fetchone()
    return {'id': row[0], 'animal_type': row[1], 'name': row[2]}


@pytest.fixture
async def authenticated_user(service_client, city):
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
    token = login_response.json()['access_token']
    org_response = await service_client.post(
        '/orgs',
        json={
            'name': f'Shelter_{uuid.uuid4().hex[:8]}',
            'city_id': city['id'],
        },
        headers={'Authorization': f'Bearer {token}'},
    )
    assert org_response.status == 201

    org_id = org_response.json()['id']
    return {'token': token, 'org_id': org_id}


@pytest.fixture
async def second_user(service_client, city):
    email = make_unique_email('owner2')
    password = 'TestPassword123'

    register_response = await service_client.post(
        '/user/register',
        json={
            'email': email,
            'password': password,
            'first_name': 'Owner2',
            'last_name': 'User',
        },
    )
    assert register_response.status == 201
    login_response = await service_client.post(
        '/auth/login',
        json={'email': email, 'password': password},
    )
    assert login_response.status == 200
    token = login_response.json()['access_token']
    org_response = await service_client.post(
        '/orgs',
        json={
            'name': f'Shelter2_{uuid.uuid4().hex[:8]}',
            'city_id': city['id'],
        },
        headers={'Authorization': f'Bearer {token}'},
    )
    assert org_response.status == 201

    org_id = org_response.json()['id']
    return {'token': token, 'org_id': org_id}


@pytest.fixture
async def registered_animal(service_client, authenticated_user, breed):
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
            'organization_id': authenticated_user['org_id'],
            'breed_id': breed['id'],
            'photos': [],
        },
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )
    assert response.status == 201
    return response.json()


async def create_review(service_client, token, animal_id, text='Initial review text'):
    response = await service_client.post(
        '/reviews',
        json={'animal_id': animal_id, 'text': text},
        headers={'Authorization': f'Bearer {token}'},
    )
    assert response.status == 201
    return response.json()


async def test_update_review_success(service_client, authenticated_user, registered_animal):
    review = await create_review(service_client, authenticated_user['token'], registered_animal['id'])

    response = await service_client.put(
        f'/reviews/{review["id"]}',
        json={'text': 'Updated review text'},
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )

    assert response.status == 200
    data = response.json()
    assert data['id'] == review['id']
    assert data['text'] == 'Updated review text'
    assert data['animal_id'] == registered_animal['id']
    assert 'animal_name' in data
    assert 'created_at' in data
    assert data['can_edit'] is True


async def test_update_review_update_keeps_other_fields(
    service_client, authenticated_user, registered_animal
):
    review = await create_review(service_client, authenticated_user['token'], registered_animal['id'])

    response = await service_client.put(
        f'/reviews/{review["id"]}',
        json={'text': 'New text only'},
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )

    assert response.status == 200
    data = response.json()
    assert data['id'] == review['id']
    assert data['animal_id'] == registered_animal['id']
    assert data['text'] == 'New text only'


async def test_update_review_persists_in_database(service_client, authenticated_user, registered_animal, pgsql):
    review = await create_review(service_client, authenticated_user['token'], registered_animal['id'])

    await service_client.put(
        f'/reviews/{review["id"]}',
        json={'text': 'After update'},
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )

    conn = pgsql['postgres-db']
    cursor = conn.cursor()
    cursor.execute(
        'SELECT id, text FROM reviews WHERE id = %s',
        (review['id'],),
    )
    row = cursor.fetchone()
    assert row is not None
    assert row[0] == review['id']
    assert row[1] == 'After update'


async def test_update_review_multiple_times(service_client, authenticated_user, registered_animal):
    review = await create_review(service_client, authenticated_user['token'], registered_animal['id'])

    response1 = await service_client.put(
        f'/reviews/{review["id"]}',
        json={'text': 'Version 2'},
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )
    assert response1.status == 200
    assert response1.json()['text'] == 'Version 2'

    response2 = await service_client.put(
        f'/reviews/{review["id"]}',
        json={'text': 'Version 3'},
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )
    assert response2.status == 200
    assert response2.json()['text'] == 'Version 3'


async def test_update_review_empty_text_returns_400(service_client, authenticated_user, registered_animal):
    review = await create_review(service_client, authenticated_user['token'], registered_animal['id'])

    response = await service_client.put(
        f'/reviews/{review["id"]}',
        json={'text': ''},
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )

    assert response.status == 400
    assert response.json()['error']['code'] == 'VALIDATION_ERROR'


async def test_update_review_whitespace_only_text_returns_400(
    service_client, authenticated_user, registered_animal
):
    review = await create_review(service_client, authenticated_user['token'], registered_animal['id'])

    response = await service_client.put(
        f'/reviews/{review["id"]}',
        json={'text': '   '},
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )

    assert response.status == 400
    assert response.json()['error']['code'] == 'VALIDATION_ERROR'


async def test_update_review_text_too_long_returns_400(service_client, authenticated_user, registered_animal):
    review = await create_review(service_client, authenticated_user['token'], registered_animal['id'])
    long_text = 'a' * 2001

    response = await service_client.put(
        f'/reviews/{review["id"]}',
        json={'text': long_text},
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )

    assert response.status == 400
    assert response.json()['error']['code'] == 'VALIDATION_ERROR'


async def test_update_review_empty_json_returns_200(service_client, authenticated_user, registered_animal):
    review = await create_review(service_client, authenticated_user['token'], registered_animal['id'])

    response = await service_client.put(
        f'/reviews/{review["id"]}',
        json={},
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )

    assert response.status == 200
    assert response.json()['text'] == review['text']


async def test_update_review_unauthenticated_returns_401(service_client, authenticated_user, registered_animal):
    review = await create_review(service_client, authenticated_user['token'], registered_animal['id'])

    response = await service_client.put(
        f'/reviews/{review["id"]}',
        json={'text': 'Updated review text'},
    )
    assert response.status == 401


async def test_update_review_invalid_token_returns_401(service_client, authenticated_user, registered_animal):
    review = await create_review(service_client, authenticated_user['token'], registered_animal['id'])

    response = await service_client.put(
        f'/reviews/{review["id"]}',
        json={'text': 'Updated review text'},
        headers={'Authorization': 'Bearer invalid-token'},
    )
    assert response.status == 401


async def test_update_review_not_found(service_client, authenticated_user):
    response = await service_client.put(
        '/reviews/999999999',
        json={'text': 'Updated review text'},
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )
    assert response.status == 404
    assert response.json()['error']['code'] == 'REVIEW_NOT_FOUND'


async def test_update_review_invalid_id(service_client, authenticated_user):
    response = await service_client.put(
        '/reviews/not-a-number',
        json={'text': 'Updated review text'},
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )
    assert response.status == 404


async def test_update_review_forbidden_wrong_user(service_client, authenticated_user, second_user, registered_animal):
    review = await create_review(service_client, authenticated_user['token'], registered_animal['id'])

    response = await service_client.put(
        f'/reviews/{review["id"]}',
        json={'text': 'Stolen text'},
        headers={'Authorization': f"Bearer {second_user['token']}"},
    )

    assert response.status == 403
    assert response.json()['error']['code'] == 'FORBIDDEN'

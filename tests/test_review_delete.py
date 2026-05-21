import uuid

import pytest


def make_unique_email(prefix='review_delete'):
    random_part = uuid.uuid4().hex[:10]
    return f'{prefix}_{random_part}@example.com'


@pytest.fixture
async def city(pgsql):
    conn = pgsql['postgres-db']
    cursor = conn.cursor()
    city_name = f'review_delete_{uuid.uuid4().hex[:8]}'
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
    breed_name = f'review_delete_{uuid.uuid4().hex[:8]}'
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


async def test_review_delete_success(service_client, authenticated_user, registered_animal):
    response = await service_client.post(
        '/reviews',
        json={'animal_id': registered_animal['id'], 'text': 'To be deleted'},
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )
    assert response.status == 201
    review = response.json()

    resp_delete = await service_client.delete(
        f"/reviews/{review['id']}",
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )
    assert resp_delete.status == 204


async def test_review_delete_not_found(service_client, authenticated_user):
    resp_delete = await service_client.delete(
        '/reviews/999999',
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )
    assert resp_delete.status == 404
    assert resp_delete.json()['error']['code'] == 'REVIEW_NOT_FOUND'


async def test_review_delete_forbidden(service_client, authenticated_user, city, breed):
    user2_email = make_unique_email('other')
    user2_password = 'TestPassword123'

    register2 = await service_client.post(
        '/user/register',
        json={
            'email': user2_email,
            'password': user2_password,
            'first_name': 'Other',
            'last_name': 'Owner',
        },
    )
    assert register2.status == 201

    login2 = await service_client.post(
        '/auth/login',
        json={'email': user2_email, 'password': user2_password},
    )
    token2 = login2.json()['access_token']

    org2 = await service_client.post(
        '/orgs',
        json={'name': f'Org2_{uuid.uuid4().hex[:8]}', 'city_id': city['id']},
        headers={'Authorization': f'Bearer {token2}'},
    )
    assert org2.status == 201
    org2_id = org2.json()['id']

    a2 = await service_client.post(
        '/animals',
        json={
            'name': 'Other',
            'size': 'large',
            'gender': 'female',
            'care_level': 'easy',
            'color': 'white',
            'good_with': 'children',
            'age': 2,
            'status': 'available',
            'organization_id': org2_id,
            'breed_id': breed['id'],
        },
        headers={'Authorization': f'Bearer {token2}'},
    )
    assert a2.status == 201
    a2_id = a2.json()['id']

    r = await service_client.post(
        '/reviews',
        json={'animal_id': a2_id, 'text': 'Other review'},
        headers={'Authorization': f'Bearer {token2}'},
    )
    assert r.status == 201
    review = r.json()

    resp_delete = await service_client.delete(
        f"/reviews/{review['id']}",
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )
    assert resp_delete.status == 403
    assert resp_delete.json()['error']['code'] == 'FORBIDDEN'

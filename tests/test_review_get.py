import math
import uuid
import pytest


def make_unique_email(prefix='review_get'):
    random_part = uuid.uuid4().hex[:10]
    return f'{prefix}_{random_part}@example.com'


@pytest.fixture
async def city(pgsql):
    conn = pgsql['postgres-db']
    cursor = conn.cursor()
    city_name = f'review_get_{uuid.uuid4().hex[:8]}'
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
    breed_name = f'review_get_{uuid.uuid4().hex[:8]}'
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
            'name': f'review_get_{uuid.uuid4().hex[:8]}',
            'city_id': city['id'],
        },
        headers={'Authorization': f"Bearer {token}"},
    )
    assert org_response.status == 201

    org_id = org_response.json()['id']
    return {'token': token, 'org_id': org_id}


@pytest.fixture
async def registered_org(service_client, authenticated_user, city):
    return {'id': authenticated_user['org_id']}


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


async def create_review(service_client, token, animal_id, text=None):
    payload = {
        'animal_id': animal_id,
        'text': text or f'Review content {uuid.uuid4().hex[:6]}',
    }
    response = await service_client.post(
        '/reviews',
        json=payload,
        headers={'Authorization': f'Bearer {token}'},
    )
    assert response.status == 201, f"Expected 201, got {response.status}: {response.text}"
    return response.json()


async def get_reviews(service_client, token, org_id, **params):
    query = [f'org_id={org_id}']
    for key, value in params.items():
        query.append(f'{key}={value}')
    return await service_client.get(
        f'/reviews?{"&".join(query)}',
        headers={'Authorization': f'Bearer {token}'},
    )


async def test_reviews_returns_200(service_client, authenticated_user):
    response = await get_reviews(
        service_client,
        authenticated_user['token'],
        authenticated_user['org_id'],
    )
    assert response.status == 200


async def test_reviews_returns_paginated_object(service_client, authenticated_user):
    response = await get_reviews(
        service_client,
        authenticated_user['token'],
        authenticated_user['org_id'],
    )
    assert response.status == 200
    data = response.json()
    assert isinstance(data, dict)
    assert 'items' in data
    assert 'page' in data
    assert 'limit' in data
    assert 'total_count' in data
    assert 'total_pages' in data


async def test_reviews_default_page_is_1(service_client, authenticated_user):
    response = await get_reviews(
        service_client,
        authenticated_user['token'],
        authenticated_user['org_id'],
    )
    assert response.status == 200
    assert response.json()['page'] == 1


async def test_reviews_default_limit(service_client, authenticated_user):
    response = await get_reviews(
        service_client,
        authenticated_user['token'],
        authenticated_user['org_id'],
    )
    assert response.status == 200
    assert response.json()['limit'] == 20


async def test_reviews_empty_org_returns_empty_items(service_client, authenticated_user):
    response = await get_reviews(
        service_client,
        authenticated_user['token'],
        authenticated_user['org_id'],
    )
    assert response.status == 200
    data = response.json()
    assert data['items'] == []
    assert data['total_count'] == 0
    assert data['total_pages'] == 1


async def test_reviews_contains_created_review(service_client, authenticated_user, registered_animal):
    review = await create_review(service_client, authenticated_user['token'], registered_animal['id'])

    response = await get_reviews(
        service_client,
        authenticated_user['token'],
        authenticated_user['org_id'],
    )
    assert response.status == 200
    ids = [r['id'] for r in response.json()['items']]
    assert review['id'] in ids


async def test_reviews_response_shape(service_client, authenticated_user, registered_animal):
    review = await create_review(service_client, authenticated_user['token'], registered_animal['id'])

    response = await get_reviews(
        service_client,
        authenticated_user['token'],
        authenticated_user['org_id'],
    )
    assert response.status == 200
    items = response.json()['items']
    item = next(r for r in items if r['id'] == review['id'])
    assert 'id' in item
    assert 'animal_id' in item
    assert 'animal_name' in item
    assert 'text' in item
    assert 'created_at' in item


async def test_reviews_only_returns_own_org_reviews(service_client, city, breed):
    user1_email = make_unique_email('owner1')
    user1_password = 'TestPassword123'

    register1 = await service_client.post(
        '/user/register',
        json={
            'email': user1_email,
            'password': user1_password,
            'first_name': 'Owner1',
            'last_name': 'User',
        },
    )
    assert register1.status == 201

    login1 = await service_client.post(
        '/auth/login',
        json={'email': user1_email, 'password': user1_password},
    )
    token1 = login1.json()['access_token']

    org1 = await service_client.post(
        '/orgs',
        json={'name': f'Org1_{uuid.uuid4().hex[:8]}', 'city_id': city['id']},
        headers={'Authorization': f'Bearer {token1}'},
    )
    assert org1.status == 201
    org1_id = org1.json()['id']

    user2_email = make_unique_email('owner2')
    user2_password = 'TestPassword123'

    register2 = await service_client.post(
        '/user/register',
        json={
            'email': user2_email,
            'password': user2_password,
            'first_name': 'Owner2',
            'last_name': 'User',
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

    a1 = await service_client.post(
        '/animals',
        json={
            'name': 'A1',
            'size': 'medium',
            'gender': 'male',
            'care_level': 'easy',
            'color': 'brown',
            'good_with': 'children',
            'age': 2,
            'status': 'available',
            'organization_id': org1_id,
            'breed_id': breed['id'],
        },
        headers={'Authorization': f'Bearer {token1}'},
    )
    assert a1.status == 201
    a1_id = a1.json()['id']
    a2 = await service_client.post(
        '/animals',
        json={
            'name': 'A2',
            'size': 'medium',
            'gender': 'male',
            'care_level': 'easy',
            'color': 'brown',
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

    r1 = await create_review(service_client, token1, a1_id)
    r2 = await create_review(service_client, token2, a2_id)

    response = await get_reviews(service_client, token1, org1_id)
    assert response.status == 200
    ids = [r['id'] for r in response.json()['items']]
    assert r1['id'] in ids
    assert r2['id'] not in ids


async def test_reviews_missing_org_id_returns_400(service_client, authenticated_user):
    response = await service_client.get(
        '/reviews',
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )
    assert response.status == 400
    assert response.json()['error']['code'] == 'MISSING_FIELD'


async def test_reviews_invalid_org_id_returns_400(service_client, authenticated_user):
    response = await service_client.get(
        '/reviews?org_id=invalid',
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )
    assert response.status == 400
    assert response.json()['error']['code'] == 'VALIDATION_ERROR'


async def test_reviews_org_not_found_returns_404(service_client, authenticated_user):
    response = await service_client.get(
        '/reviews?org_id=999999',
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )
    assert response.status == 404
    assert response.json()['error']['code'] == 'ORGANIZATION_NOT_FOUND'


async def test_reviews_invalid_page_returns_400(service_client, authenticated_user):
    response = await get_reviews(
        service_client,
        authenticated_user['token'],
        authenticated_user['org_id'],
        page='abc',
    )
    assert response.status == 400


async def test_reviews_negative_page_returns_400(service_client, authenticated_user):
    org_id = authenticated_user['org_id']
    response_page0 = await get_reviews(
        service_client,
        authenticated_user['token'],
        org_id,
        page=0,
    )
    response_page_neg = await get_reviews(
        service_client,
        authenticated_user['token'],
        org_id,
        page=-1,
    )
    assert response_page0.status == 400
    assert response_page_neg.status == 400

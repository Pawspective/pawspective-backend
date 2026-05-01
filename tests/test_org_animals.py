import math
import uuid

import pytest


def make_unique_email(prefix='org_animal'):
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
            'name': f'Shelter_{uuid.uuid4().hex[:8]}',
            'city_id': city['id'],
        },
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )
    assert response.status == 201
    return response.json()


async def create_animal(service_client, token, org_id, breed_id, overrides=None):
    payload = {
        'name': f'Animal_{uuid.uuid4().hex[:6]}',
        'organization_id': org_id,
        'breed_id': breed_id,
        'size': 'medium',
        'gender': 'male',
        'care_level': 'easy',
        'color': 'black',
        'good_with': 'dogs',
        'age': 3,
        'status': 'available',
    }
    if overrides:
        payload.update(overrides)
    response = await service_client.post(
        '/animals',
        json=payload,
        headers={'Authorization': f'Bearer {token}'},
    )
    assert response.status == 201
    return response.json()


# --- Basic response structure ---

async def test_org_animals_returns_200(
    service_client, registered_org,
):
    """GET /orgs/{id}/animals returns 200"""
    response = await service_client.get(f'/orgs/{registered_org["id"]}/animals')
    assert response.status == 200


async def test_org_animals_returns_paginated_object(
    service_client, registered_org,
):
    """GET /orgs/{id}/animals returns a paginated object, not a bare array"""
    response = await service_client.get(f'/orgs/{registered_org["id"]}/animals')
    assert response.status == 200
    data = response.json()
    assert isinstance(data, dict)
    assert 'items' in data
    assert 'page' in data
    assert 'limit' in data
    assert 'total_count' in data
    assert 'total_pages' in data


async def test_org_animals_default_page_is_1(
    service_client, registered_org,
):
    """Default page is 1 when not specified"""
    response = await service_client.get(f'/orgs/{registered_org["id"]}/animals')
    assert response.status == 200
    assert response.json()['page'] == 1


async def test_org_animals_default_limit(
    service_client, registered_org,
):
    """Default limit is 20"""
    response = await service_client.get(f'/orgs/{registered_org["id"]}/animals')
    assert response.status == 200
    assert response.json()['limit'] == 20


async def test_org_animals_empty_org_returns_empty_items(
    service_client, registered_org,
):
    """Organization with no animals returns empty items and zero total_count"""
    response = await service_client.get(f'/orgs/{registered_org["id"]}/animals')
    assert response.status == 200
    data = response.json()
    assert data['items'] == []
    assert data['total_count'] == 0
    assert data['total_pages'] == 1


async def test_org_animals_contains_created_animal(
    service_client, authenticated_user, registered_org, breed,
):
    """Animals registered under the org appear in its animal list"""
    animal = await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        breed['id'],
    )

    response = await service_client.get(f'/orgs/{registered_org["id"]}/animals')
    assert response.status == 200
    ids = [a['id'] for a in response.json()['items']]
    assert animal['id'] in ids


async def test_org_animals_response_shape(
    service_client, authenticated_user, registered_org, breed,
):
    """Each animal in the list has the expected fields"""
    animal = await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        breed['id'],
        {'name': 'Shaper', 'color': 'brown', 'age': 2},
    )

    response = await service_client.get(f'/orgs/{registered_org["id"]}/animals')
    assert response.status == 200
    items = response.json()['items']
    item = next(a for a in items if a['id'] == animal['id'])
    assert 'id' in item
    assert 'name' in item
    assert 'organization_id' in item
    assert 'breed' in item
    assert 'size' in item
    assert 'gender' in item
    assert 'care_level' in item
    assert 'color' in item
    assert 'good_with' in item
    assert 'age' in item
    assert 'status' in item


async def test_org_animals_only_returns_own_animals(
    service_client, authenticated_user, city, breed, pgsql,
):
    """Animals from another org do not appear in this org's list"""
    user2_email = make_unique_email('owner2')
    user2_password = 'TestPassword123'
    await service_client.post(
        '/user/register',
        json={
            'email': user2_email,
            'password': user2_password,
            'first_name': 'Owner2',
            'last_name': 'User',
        },
    )
    login2 = await service_client.post(
        '/auth/login',
        json={'email': user2_email, 'password': user2_password},
    )
    token2 = login2.json()['access_token']

    org1 = (
        await service_client.post(
            '/orgs',
            json={
                'name': f'Org1_{uuid.uuid4().hex[:8]}', 'city_id': city['id']},
            headers={'Authorization': f"Bearer {authenticated_user['token']}"},
        )
    ).json()
    org2 = (
        await service_client.post(
            '/orgs',
            json={
                'name': f'Org2_{uuid.uuid4().hex[:8]}', 'city_id': city['id']},
            headers={'Authorization': f'Bearer {token2}'},
        )
    ).json()

    animal1 = await create_animal(
        service_client, authenticated_user['token'], org1['id'], breed['id'],
    )
    animal2 = await create_animal(service_client, token2, org2['id'], breed['id'])

    response = await service_client.get(f'/orgs/{org1["id"]}/animals')
    assert response.status == 200
    ids = [a['id'] for a in response.json()['items']]
    assert animal1['id'] in ids
    assert animal2['id'] not in ids


# --- Pagination ---

async def test_org_animals_page_param_accepted(
    service_client, registered_org,
):
    """?page=1 is accepted and returns page 1"""
    response = await service_client.get(
        f'/orgs/{registered_org["id"]}/animals?page=1',
    )
    assert response.status == 200
    assert response.json()['page'] == 1


async def test_org_animals_total_count_reflects_created(
    service_client, authenticated_user, registered_org, breed,
):
    """total_count increases after creating animals"""
    before = (
        await service_client.get(f'/orgs/{registered_org["id"]}/animals')
    ).json()['total_count']

    await create_animal(
        service_client, authenticated_user['token'], registered_org['id'], breed['id'],
    )
    await create_animal(
        service_client, authenticated_user['token'], registered_org['id'], breed['id'],
    )

    after = (
        await service_client.get(f'/orgs/{registered_org["id"]}/animals')
    ).json()['total_count']
    assert after == before + 2


async def test_org_animals_total_pages_calculated_correctly(
    service_client, authenticated_user, registered_org, breed,
):
    """total_pages = ceil(total_count / limit)"""
    for _ in range(3):
        await create_animal(
            service_client,
            authenticated_user['token'],
            registered_org['id'],
            breed['id'],
        )

    response = await service_client.get(f'/orgs/{registered_org["id"]}/animals')
    data = response.json()
    expected_pages = max(1, math.ceil(data['total_count'] / data['limit']))
    assert data['total_pages'] == expected_pages


async def test_org_animals_page_2_different_from_page_1(
    service_client, authenticated_user, registered_org, breed,
):
    """When there are more than 20 animals, page 2 differs from page 1"""
    for _ in range(22):
        await create_animal(
            service_client,
            authenticated_user['token'],
            registered_org['id'],
            breed['id'],
        )

    page1 = await service_client.get(
        f'/orgs/{registered_org["id"]}/animals?page=1',
    )
    page2 = await service_client.get(
        f'/orgs/{registered_org["id"]}/animals?page=2',
    )

    assert page1.status == 200
    assert page2.status == 200

    ids1 = {a['id'] for a in page1.json()['items']}
    ids2 = {a['id'] for a in page2.json()['items']}

    assert len(ids1) == 20
    assert len(ids2) > 0
    assert ids1.isdisjoint(ids2)


async def test_org_animals_page_beyond_last_returns_empty(
    service_client, registered_org,
):
    """Requesting a page far beyond total_pages returns empty items"""
    response = await service_client.get(
        f'/orgs/{registered_org["id"]}/animals?page=999999',
    )
    assert response.status == 200
    data = response.json()
    assert data['items'] == []
    assert data['page'] == 999999


async def test_org_animals_items_count_on_last_page(
    service_client, authenticated_user, registered_org, breed,
):
    """Last page may have fewer than limit items"""
    before = (
        await service_client.get(f'/orgs/{registered_org["id"]}/animals')
    ).json()['total_count']

    needed = 21 - (before % 20) if before % 20 != 0 else 1
    for _ in range(needed):
        await create_animal(
            service_client,
            authenticated_user['token'],
            registered_org['id'],
            breed['id'],
        )

    data = (
        await service_client.get(f'/orgs/{registered_org["id"]}/animals')
    ).json()
    total_pages = data['total_pages']

    last_page = (
        await service_client.get(
            f'/orgs/{registered_org["id"]}/animals?page={total_pages}',
        )
    ).json()
    remainder = data['total_count'] % data['limit']
    expected_last = remainder if remainder != 0 else data['limit']
    assert len(last_page['items']) == expected_last


# --- Error handling ---

async def test_org_animals_not_found(service_client):
    """Getting animals for a non-existent organization returns 404"""
    response = await service_client.get('/orgs/999999/animals')
    assert response.status == 404
    assert response.json()['error']['code'] == 'ORGANIZATION_NOT_FOUND'


async def test_org_animals_invalid_id(service_client):
    """Invalid (non-integer) org ID in path returns 404"""
    response = await service_client.get('/orgs/invalid_id/animals')
    assert response.status == 404
    assert response.json()['error']['code'] == 'ORGANIZATION_NOT_FOUND'


async def test_org_animals_invalid_page_returns_400(
    service_client, registered_org,
):
    """Non-integer page param returns 400"""
    response = await service_client.get(
        f'/orgs/{registered_org["id"]}/animals?page=abc',
    )
    assert response.status == 400


async def test_org_animals_negative_page_returns_400(
    service_client, registered_org,
):
    """page=0 and negative page return 400"""
    org_id = registered_org['id']
    assert (
        await service_client.get(f'/orgs/{org_id}/animals?page=0')
    ).status == 400
    assert (
        await service_client.get(f'/orgs/{org_id}/animals?page=-1')
    ).status == 400

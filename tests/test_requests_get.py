import math
import uuid

import pytest


def make_unique_email(prefix='req_user'):
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


async def register_and_login(service_client, city_id, prefix='org_owner'):
    email = make_unique_email(prefix)
    password = 'TestPassword123'

    reg = await service_client.post(
        '/user/register',
        json={
            'email': email,
            'password': password,
            'first_name': 'Test',
            'last_name': 'User',
        },
    )
    assert reg.status == 201

    login = await service_client.post(
        '/auth/login',
        json={'email': email, 'password': password},
    )
    assert login.status == 200
    token = login.json()['access_token']

    org = await service_client.post(
        '/orgs',
        json={'name': f'Shelter_{uuid.uuid4().hex[:8]}', 'city_id': city_id},
        headers={'Authorization': f'Bearer {token}'},
    )
    assert org.status == 201
    org_id = org.json()['id']

    return {'token': token, 'org_id': org_id, 'email': email}


@pytest.fixture
async def org_owner(service_client, city):
    return await register_and_login(service_client, city['id'])


async def test_requests_returns_200(service_client, org_owner):
    """GET /requests?org_id={id} returns 200 for org member"""
    response = await service_client.get(
        f'/requests?org_id={org_owner["org_id"]}',
        headers={'Authorization': f'Bearer {org_owner["token"]}'},
    )
    assert response.status == 200


async def test_requests_returns_paginated_object(service_client, org_owner):
    """GET /requests returns a paginated object"""
    response = await service_client.get(
        f'/requests?org_id={org_owner["org_id"]}',
        headers={'Authorization': f'Bearer {org_owner["token"]}'},
    )
    assert response.status == 200
    data = response.json()
    assert isinstance(data, dict)
    assert 'items' in data
    assert 'page' in data
    assert 'limit' in data
    assert 'total_count' in data
    assert 'total_pages' in data


async def test_requests_empty_org_returns_empty_items(service_client, org_owner):
    """Organization with no requests returns empty items"""
    response = await service_client.get(
        f'/requests?org_id={org_owner["org_id"]}',
        headers={'Authorization': f'Bearer {org_owner["token"]}'},
    )
    assert response.status == 200
    data = response.json()
    assert data['items'] == []
    assert data['total_count'] == 0
    assert data['total_pages'] == 1


async def test_requests_default_page_is_1(service_client, org_owner):
    """Default page is 1"""
    response = await service_client.get(
        f'/requests?org_id={org_owner["org_id"]}',
        headers={'Authorization': f'Bearer {org_owner["token"]}'},
    )
    assert response.status == 200
    assert response.json()['page'] == 1


async def test_requests_default_limit(service_client, org_owner):
    """Default limit is 20"""
    response = await service_client.get(
        f'/requests?org_id={org_owner["org_id"]}',
        headers={'Authorization': f'Bearer {org_owner["token"]}'},
    )
    assert response.status == 200
    assert response.json()['limit'] == 20


async def test_requests_page_param_accepted(service_client, org_owner):
    """?page=1 is accepted"""
    response = await service_client.get(
        f'/requests?org_id={org_owner["org_id"]}&page=1',
        headers={'Authorization': f'Bearer {org_owner["token"]}'},
    )
    assert response.status == 200
    assert response.json()['page'] == 1


async def test_requests_without_auth_returns_401(service_client, org_owner):
    """Request without auth token returns 401"""
    response = await service_client.get(
        f'/requests?org_id={org_owner["org_id"]}',
    )
    assert response.status == 401


async def test_requests_non_member_returns_403(service_client, city, org_owner):
    """User who is not an org member gets 403"""
    other = await register_and_login(service_client, city['id'], prefix='other')
    response = await service_client.get(
        f'/requests?org_id={org_owner["org_id"]}',
        headers={'Authorization': f'Bearer {other["token"]}'},
    )
    assert response.status == 403


async def test_requests_missing_org_id_returns_400(service_client, org_owner):
    """Request without org_id returns 400"""
    response = await service_client.get(
        '/requests',
        headers={'Authorization': f'Bearer {org_owner["token"]}'},
    )
    assert response.status == 400
    assert response.json()['error']['code'] == 'MISSING_FIELD'


async def test_requests_invalid_org_id_returns_400(service_client, org_owner):
    """Non-integer org_id returns 400"""
    response = await service_client.get(
        '/requests?org_id=invalid',
        headers={'Authorization': f'Bearer {org_owner["token"]}'},
    )
    assert response.status == 400
    assert response.json()['error']['code'] == 'VALIDATION_ERROR'


async def test_requests_negative_org_id_returns_400(service_client, org_owner):
    """Negative org_id returns 400"""
    response = await service_client.get(
        '/requests?org_id=-1',
        headers={'Authorization': f'Bearer {org_owner["token"]}'},
    )
    assert response.status == 400


async def test_requests_zero_org_id_returns_400(service_client, org_owner):
    """Zero org_id returns 400"""
    response = await service_client.get(
        '/requests?org_id=0',
        headers={'Authorization': f'Bearer {org_owner["token"]}'},
    )
    assert response.status == 400


async def test_requests_invalid_page_returns_400(service_client, org_owner):
    """Non-integer page returns 400"""
    response = await service_client.get(
        f'/requests?org_id={org_owner["org_id"]}&page=abc',
        headers={'Authorization': f'Bearer {org_owner["token"]}'},
    )
    assert response.status == 400


async def test_requests_negative_page_returns_400(service_client, org_owner):
    """page=0 and negative page return 400"""
    org_id = org_owner['org_id']
    token = org_owner['token']
    r0 = await service_client.get(
        f'/requests?org_id={org_id}&page=0',
        headers={'Authorization': f'Bearer {token}'},
    )
    r_neg = await service_client.get(
        f'/requests?org_id={org_id}&page=-1',
        headers={'Authorization': f'Bearer {token}'},
    )
    assert r0.status == 400
    assert r_neg.status == 400

import math
import uuid

import pytest


def make_unique_email(prefix='post_user'):
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


async def create_post(service_client, token, org_id, overrides=None):
    payload = {
        'text': f'Post content {uuid.uuid4().hex[:6]}',
    }
    if overrides:
        payload.update(overrides)
    response = await service_client.post(
        '/posts',
        json=payload,
        headers={'Authorization': f'Bearer {token}'},
    )
    assert response.status == 201
    return response.json()


async def test_posts_returns_200(service_client, registered_org):
    """GET /posts?org_id={id} returns 200"""
    response = await service_client.get(f'/posts?org_id={registered_org["id"]}')
    assert response.status == 200


async def test_posts_returns_paginated_object(service_client, registered_org):
    """GET /posts?org_id={id} returns a paginated object, not a bare array"""
    response = await service_client.get(f'/posts?org_id={registered_org["id"]}')
    assert response.status == 200
    data = response.json()
    assert isinstance(data, dict)
    assert 'items' in data
    assert 'page' in data
    assert 'limit' in data
    assert 'total_count' in data
    assert 'total_pages' in data


async def test_posts_default_page_is_1(service_client, registered_org):
    """Default page is 1 when not specified"""
    response = await service_client.get(f'/posts?org_id={registered_org["id"]}')
    assert response.status == 200
    assert response.json()['page'] == 1


async def test_posts_default_limit(service_client, registered_org):
    """Default limit is 20"""
    response = await service_client.get(f'/posts?org_id={registered_org["id"]}')
    assert response.status == 200
    assert response.json()['limit'] == 20


async def test_posts_empty_org_returns_empty_items(service_client, registered_org):
    """Organization with no posts returns empty items and zero total_count"""
    response = await service_client.get(f'/posts?org_id={registered_org["id"]}')
    assert response.status == 200
    data = response.json()
    assert data['items'] == []
    assert data['total_count'] == 0
    assert data['total_pages'] == 1


async def test_posts_contains_created_post(
    service_client, authenticated_user, registered_org,
):
    """Posts created under the org appear in its post list"""
    post = await create_post(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
    )

    response = await service_client.get(f'/posts?org_id={registered_org["id"]}')
    assert response.status == 200
    ids = [p['id'] for p in response.json()['items']]
    assert post['id'] in ids


async def test_posts_response_shape(
    service_client, authenticated_user, registered_org,
):
    """Each post in the list has the expected fields"""
    post = await create_post(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        {'text': 'Test post content'},
    )

    response = await service_client.get(f'/posts?org_id={registered_org["id"]}')
    assert response.status == 200
    items = response.json()['items']
    item = next(p for p in items if p['id'] == post['id'])
    assert 'id' in item
    assert 'text' in item
    assert 'created_at' in item
    assert 'organization_id' in item


async def test_posts_only_returns_own_posts(
    service_client, authenticated_user, city,
):
    """Posts from another org do not appear in this org's list"""
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

    post1 = await create_post(
        service_client, authenticated_user['token'], org1['id'],
    )
    post2 = await create_post(service_client, token2, org2['id'])

    response = await service_client.get(f'/posts?org_id={org1["id"]}')
    assert response.status == 200
    ids = [p['id'] for p in response.json()['items']]
    assert post1['id'] in ids
    assert post2['id'] not in ids


async def test_posts_page_param_accepted(service_client, registered_org):
    """?page=1 is accepted and returns page 1"""
    response = await service_client.get(
        f'/posts?org_id={registered_org["id"]}&page=1',
    )
    assert response.status == 200
    assert response.json()['page'] == 1


async def test_posts_total_count_reflects_created(
    service_client, authenticated_user, registered_org,
):
    """total_count increases after creating posts"""
    before = (
        await service_client.get(f'/posts?org_id={registered_org["id"]}')
    ).json()['total_count']

    await create_post(
        service_client, authenticated_user['token'], registered_org['id'],
    )
    await create_post(
        service_client, authenticated_user['token'], registered_org['id'],
    )

    after = (
        await service_client.get(f'/posts?org_id={registered_org["id"]}')
    ).json()['total_count']
    assert after == before + 2


async def test_posts_total_pages_calculated_correctly(
    service_client, authenticated_user, registered_org,
):
    """total_pages = ceil(total_count / limit)"""
    for _ in range(3):
        await create_post(
            service_client,
            authenticated_user['token'],
            registered_org['id'],
        )

    response = await service_client.get(f'/posts?org_id={registered_org["id"]}')
    data = response.json()
    expected_pages = max(1, math.ceil(data['total_count'] / data['limit']))
    assert data['total_pages'] == expected_pages


async def test_posts_page_2_different_from_page_1(
    service_client, authenticated_user, registered_org,
):
    """When there are more than 20 posts, page 2 differs from page 1"""
    for _ in range(22):
        await create_post(
            service_client,
            authenticated_user['token'],
            registered_org['id'],
        )

    page1 = await service_client.get(
        f'/posts?org_id={registered_org["id"]}&page=1',
    )
    page2 = await service_client.get(
        f'/posts?org_id={registered_org["id"]}&page=2',
    )

    assert page1.status == 200
    assert page2.status == 200

    ids1 = {p['id'] for p in page1.json()['items']}
    ids2 = {p['id'] for p in page2.json()['items']}

    assert len(ids1) == 20
    assert len(ids2) > 0
    assert ids1.isdisjoint(ids2)


async def test_posts_page_beyond_last_returns_empty(service_client, registered_org):
    """Requesting a page far beyond total_pages returns empty items"""
    response = await service_client.get(
        f'/posts?org_id={registered_org["id"]}&page=999999',
    )
    assert response.status == 200
    data = response.json()
    assert data['items'] == []
    assert data['page'] == 999999


async def test_posts_items_count_on_last_page(
    service_client, authenticated_user, registered_org,
):
    """Last page may have fewer than limit items"""
    before = (
        await service_client.get(f'/posts?org_id={registered_org["id"]}')
    ).json()['total_count']

    needed = 21 - (before % 20) if before % 20 != 0 else 1
    for _ in range(needed):
        await create_post(
            service_client,
            authenticated_user['token'],
            registered_org['id'],
        )

    data = (
        await service_client.get(f'/posts?org_id={registered_org["id"]}')
    ).json()
    total_pages = data['total_pages']

    last_page = (
        await service_client.get(
            f'/posts?org_id={registered_org["id"]}&page={total_pages}',
        )
    ).json()
    remainder = data['total_count'] % data['limit']
    expected_last = remainder if remainder != 0 else data['limit']
    assert len(last_page['items']) == expected_last


async def test_posts_sorted_by_created_at_desc(
    service_client, authenticated_user, registered_org,
):
    """Posts should be sorted by created_at descending (newest first)"""
    post1 = await create_post(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        {'text': 'First post'},
    )
    post2 = await create_post(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        {'text': 'Second post'},
    )
    post3 = await create_post(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        {'text': 'Third post'},
    )

    response = await service_client.get(f'/posts?org_id={registered_org["id"]}')
    assert response.status == 200
    items = response.json()['items']
    our_posts = [p for p in items if p['id'] in {
        post1['id'], post2['id'], post3['id']}]
    assert our_posts[0]['id'] == post3['id']
    assert our_posts[1]['id'] == post2['id']
    assert our_posts[2]['id'] == post1['id']


async def test_posts_missing_org_id_returns_400(service_client):
    """Request without org_id returns 400"""
    response = await service_client.get('/posts')
    assert response.status == 400
    assert response.json()['error']['code'] == 'MISSING_FIELD'


async def test_posts_invalid_org_id_returns_400(service_client):
    """Invalid (non-integer) org_id returns 400"""
    response = await service_client.get('/posts?org_id=invalid')
    assert response.status == 400
    assert response.json()['error']['code'] == 'VALIDATION_ERROR'


async def test_posts_org_not_found_returns_404(service_client):
    """Non-existent organization returns 404"""
    response = await service_client.get('/posts?org_id=999999')
    assert response.status == 404
    assert response.json()['error']['code'] == 'ORGANIZATION_NOT_FOUND'


async def test_posts_negative_org_id_returns_400(service_client):
    """Negative org_id returns 400"""
    response = await service_client.get('/posts?org_id=-1')
    assert response.status == 400


async def test_posts_zero_org_id_returns_400(service_client):
    """Zero org_id returns 400"""
    response = await service_client.get('/posts?org_id=0')
    assert response.status == 400


async def test_posts_invalid_page_returns_400(service_client, registered_org):
    """Non-integer page param returns 400"""
    response = await service_client.get(
        f'/posts?org_id={registered_org["id"]}&page=abc',
    )
    assert response.status == 400


async def test_posts_negative_page_returns_400(service_client, registered_org):
    """page=0 and negative page return 400"""
    org_id = registered_org['id']
    response_page0 = await service_client.get(f'/posts?org_id={org_id}&page=0')
    response_page_neg = await service_client.get(f'/posts?org_id={org_id}&page=-1')
    assert response_page0.status == 400
    assert response_page_neg.status == 400

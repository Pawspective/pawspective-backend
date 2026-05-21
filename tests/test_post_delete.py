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

    return {'token': token, 'org_id': org_id, 'user_id': register_response.json()['id']}


@pytest.fixture
async def second_user(service_client, city):
    """Create another user with their own organization"""
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

    return {'token': token, 'org_id': org_id, 'user_id': register_response.json()['id']}


async def create_post(service_client, token, overrides=None):
    """Create post for the user's own organization"""
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


async def test_delete_post_success(service_client, authenticated_user):
    """DELETE /posts/{id} successfully deletes a post"""
    post = await create_post(service_client, authenticated_user['token'], {'text': 'To be deleted'})

    response = await service_client.delete(
        f'/posts/{post["id"]}',
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )

    assert response.status == 204


async def test_delete_post_removes_from_database(service_client, authenticated_user, pgsql):
    """Deleted post is actually removed from database"""
    post = await create_post(service_client, authenticated_user['token'], {'text': 'To be deleted'})

    conn = pgsql['postgres-db']
    cursor = conn.cursor()
    cursor.execute('SELECT id FROM posts WHERE id = %s', (post['id'],))
    assert cursor.fetchone() is not None

    await service_client.delete(
        f'/posts/{post["id"]}',
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )
    cursor.execute('SELECT id FROM posts WHERE id = %s', (post['id'],))
    assert cursor.fetchone() is None


async def test_delete_post_cannot_delete_twice(service_client, authenticated_user):
    """Deleting the same post twice returns 404"""
    post = await create_post(service_client, authenticated_user['token'], {'text': 'To be deleted'})

    response1 = await service_client.delete(
        f'/posts/{post["id"]}',
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )
    assert response1.status == 204
    response2 = await service_client.delete(
        f'/posts/{post["id"]}',
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )
    assert response2.status == 404


async def test_delete_post_unauthenticated_returns_401(service_client):
    """Request without auth token returns 401"""
    response = await service_client.delete('/posts/1')
    assert response.status == 401


async def test_delete_post_invalid_token_returns_401(service_client):
    """Request with invalid token returns 401"""
    response = await service_client.delete(
        '/posts/1',
        headers={'Authorization': 'Bearer invalid_token'},
    )
    assert response.status == 401


async def test_delete_post_forbidden_for_other_user(service_client, authenticated_user, second_user):
    """User cannot delete post belonging to another user's organization"""
    post = await create_post(service_client, authenticated_user['token'], {'text': 'Someone else post'})

    response = await service_client.delete(
        f'/posts/{post["id"]}',
        headers={'Authorization': f"Bearer {second_user['token']}"},
    )

    assert response.status == 403
    assert response.json()['error']['code'] == 'FORBIDDEN'

    response = await service_client.get(f'/posts?org_id={authenticated_user["org_id"]}')
    assert response.status == 200
    ids = [p['id'] for p in response.json()['items']]
    assert post['id'] in ids


async def test_delete_post_user_without_org_returns_403(service_client, authenticated_user, city):
    """User without organization cannot delete posts"""
    post = await create_post(service_client, authenticated_user['token'], {'text': 'Original post'})

    email = make_unique_email('no_org')
    password = 'TestPassword123'

    await service_client.post(
        '/user/register',
        json={
            'email': email,
            'password': password,
            'first_name': 'NoOrg',
            'last_name': 'User',
        },
    )
    login = await service_client.post(
        '/auth/login',
        json={'email': email, 'password': password},
    )
    token = login.json()['access_token']
    response = await service_client.delete(
        f'/posts/{post["id"]}',
        headers={'Authorization': f'Bearer {token}'},
    )

    assert response.status == 403
    assert response.json()['error']['code'] == 'FORBIDDEN' or response.json()[
        'error']['code'] == 'FORBIDDEN'


async def test_delete_post_not_found_returns_404(service_client, authenticated_user):
    """Deleting non-existent post returns 404"""
    response = await service_client.delete(
        '/posts/999999',
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )

    assert response.status == 404
    assert response.json()['error']['code'] == 'POST_NOT_FOUND'


async def test_delete_post_invalid_id_returns_400(service_client, authenticated_user):
    """Deleting with invalid post ID returns 400"""
    response = await service_client.delete(
        '/posts/invalid_id',
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )

    assert response.status == 400
    assert response.json()['error']['code'] == 'POST_NOT_FOUND'


async def test_delete_post_after_organization_deleted(service_client, authenticated_user, city):
    """Deleting a post after organization is deleted should handle gracefully"""
    post = await create_post(service_client, authenticated_user['token'], {'text': 'Post'})
    response = await service_client.delete(
        f'/orgs/{authenticated_user["org_id"]}',
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )
    assert response.status == 204
    response = await service_client.delete(
        f'/posts/{post["id"]}',
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )
    assert response.status == 404

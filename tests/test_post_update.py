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
        'photos': [],
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


async def test_update_post_success(service_client, authenticated_user):
    """PUT /posts/{id} successfully updates a post"""
    post = await create_post(service_client, authenticated_user['token'], {'text': 'Original text'})

    response = await service_client.put(
        f'/posts/{post["id"]}',
        json={'text': 'Updated text'},
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )

    assert response.status == 200
    data = response.json()
    assert data['id'] == post['id']
    assert data['text'] == 'Updated text'
    assert data['organization_id'] == authenticated_user['org_id']
    assert 'created_at' in data
    assert 'photos' in data
    assert data['photos'] == []


async def test_update_post_partial_update(service_client, authenticated_user):
    """PUT with only text field updates only the text"""
    post = await create_post(service_client, authenticated_user['token'], {'text': 'Original text'})

    response = await service_client.put(
        f'/posts/{post["id"]}',
        json={'text': 'New text only'},
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )

    assert response.status == 200
    data = response.json()
    assert data['text'] == 'New text only'
    assert data['organization_id'] == authenticated_user['org_id']
    assert 'photos' in data
    assert data['photos'] == []


async def test_update_post_persists_in_database(service_client, authenticated_user, pgsql):
    """Updated post is actually saved to database"""
    post = await create_post(service_client, authenticated_user['token'], {'text': 'Before update'})

    await service_client.put(
        f'/posts/{post["id"]}',
        json={'text': 'After update', 'photos': []},
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )
    conn = pgsql['postgres-db']
    cursor = conn.cursor()
    cursor.execute(
        'SELECT id, text, photos FROM posts WHERE id = %s',
        (post['id'],),
    )
    row = cursor.fetchone()
    assert row is not None
    assert row[0] == post['id']
    assert row[1] == 'After update'
    assert row[2] == []


async def test_update_post_multiple_times(service_client, authenticated_user):
    """Post can be updated multiple times"""
    post = await create_post(service_client, authenticated_user['token'], {'text': 'Version 1'})

    response1 = await service_client.put(
        f'/posts/{post["id"]}',
        json={'text': 'Version 2', 'photos': []},
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )
    assert response1.status == 200
    assert response1.json()['text'] == 'Version 2'
    assert response1.json()['photos'] == []
    response2 = await service_client.put(
        f'/posts/{post["id"]}',
        json={'text': 'Version 3', 'photos': []},
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )
    assert response2.status == 200
    assert response2.json()['text'] == 'Version 3'
    assert response2.json()['photos'] == []


async def test_update_post_empty_text_returns_400(service_client, authenticated_user):
    """Updating with empty text returns 400"""
    post = await create_post(service_client, authenticated_user['token'], {'text': 'Original'})

    response = await service_client.put(
        f'/posts/{post["id"]}',
        json={'text': ''},
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )

    assert response.status == 400
    assert response.json()['error']['code'] == 'VALIDATION_ERROR'


async def test_update_post_whitespace_only_text_returns_400(service_client, authenticated_user):
    """Updating with whitespace-only text returns 400"""
    post = await create_post(service_client, authenticated_user['token'], {'text': 'Original'})

    response = await service_client.put(
        f'/posts/{post["id"]}',
        json={'text': '   '},
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )

    assert response.status == 400
    assert response.json()['error']['code'] == 'VALIDATION_ERROR'


async def test_update_post_text_too_long_returns_400(service_client, authenticated_user):
    """Updating with text longer than 5000 characters returns 400"""
    post = await create_post(service_client, authenticated_user['token'], {'text': 'Original'})
    long_text = 'a' * 5001

    response = await service_client.put(
        f'/posts/{post["id"]}',
        json={'text': long_text},
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )

    assert response.status == 400
    assert response.json()['error']['code'] == 'VALIDATION_ERROR'


async def test_update_post_empty_json_returns_200(service_client, authenticated_user):
    """Updating with empty JSON (no fields) returns 200 (no changes)"""
    post = await create_post(service_client, authenticated_user['token'], {'text': 'Original'})

    response = await service_client.put(
        f'/posts/{post["id"]}',
        json={},
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )

    assert response.status == 200
    data = response.json()
    assert data['text'] == 'Original'


async def test_update_post_unauthenticated_returns_401(service_client):
    """Request without auth token returns 401"""
    response = await service_client.put(
        '/posts/1',
        json={'text': 'Updated text'},
    )
    assert response.status == 401


async def test_update_post_invalid_token_returns_401(service_client):
    """Request with invalid token returns 401"""
    response = await service_client.put(
        '/posts/1',
        json={'text': 'Updated text'},
        headers={'Authorization': 'Bearer invalid_token'},
    )
    assert response.status == 401


async def test_update_post_forbidden_for_other_user(service_client, authenticated_user, second_user):
    """User cannot update post belonging to another user's organization"""
    post = await create_post(service_client, authenticated_user['token'], {'text': 'Original'})

    response = await service_client.put(
        f'/posts/{post["id"]}',
        json={'text': 'Hacked!'},
        headers={'Authorization': f"Bearer {second_user['token']}"},
    )

    assert response.status == 403
    assert response.json()['error']['code'] == 'FORBIDDEN' or response.json()[
        'error']['code'] == 'FORBIDDEN'


async def test_update_post_user_without_org_returns_403(service_client, city, authenticated_user):
    """User without organization cannot update posts"""
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
    response = await service_client.put(
        f'/posts/{post["id"]}',
        json={'text': 'Updated'},
        headers={'Authorization': f'Bearer {token}'},
    )
    assert response.status == 403
    assert response.json()['error']['code'] == 'FORBIDDEN'


async def test_update_post_not_found_returns_404(service_client, authenticated_user):
    """Updating non-existent post returns 404"""
    response = await service_client.put(
        '/posts/999999',
        json={'text': 'Updated text'},
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )

    assert response.status == 404
    assert response.json()['error']['code'] == 'POST_NOT_FOUND'


async def test_update_post_invalid_id_returns_404(service_client, authenticated_user):
    """Updating with invalid post ID returns 404"""
    response = await service_client.put(
        '/posts/invalid_id',
        json={'text': 'Updated text'},
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )

    assert response.status == 404


async def test_update_post_unicode_text(service_client, authenticated_user):
    """Post can be updated with unicode characters"""
    post = await create_post(service_client, authenticated_user['token'], {'text': 'Original'})
    unicode_text = 'Привет мир!'

    response = await service_client.put(
        f'/posts/{post["id"]}',
        json={'text': unicode_text},
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )

    assert response.status == 200
    assert response.json()['text'] == unicode_text


async def test_update_post_special_characters(service_client, authenticated_user):
    """Post can be updated with special characters"""
    post = await create_post(service_client, authenticated_user['token'], {'text': 'Original'})
    special_text = 'Test! @#$%^&*()_+{}:"<>?|~`'

    response = await service_client.put(
        f'/posts/{post["id"]}',
        json={'text': special_text},
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )

    assert response.status == 200
    assert response.json()['text'] == special_text


async def test_update_post_keeps_original_created_at(service_client, authenticated_user):
    """created_at timestamp remains unchanged after update"""
    import time

    post = await create_post(service_client, authenticated_user['token'], {'text': 'Original'})
    original_created_at = post['created_at']
    import asyncio
    await asyncio.sleep(0.1)

    response = await service_client.put(
        f'/posts/{post["id"]}',
        json={'text': 'Updated'},
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )

    assert response.status == 200
    assert response.json()['created_at'] == original_created_at


async def test_update_post_same_text(service_client, authenticated_user):
    """Updating with same text succeeds"""
    post = await create_post(service_client, authenticated_user['token'], {'text': 'Same text'})

    response = await service_client.put(
        f'/posts/{post["id"]}',
        json={'text': 'Same text'},
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )

    assert response.status == 200
    assert response.json()['text'] == 'Same text'

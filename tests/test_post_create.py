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


async def test_create_post_success(service_client, authenticated_user, registered_org):
    """POST /posts returns 201 and creates a post"""
    post_text = f'Test post content {uuid.uuid4().hex[:6]}'

    response = await service_client.post(
        '/posts',
        json={'text': post_text},
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )

    assert response.status == 201
    data = response.json()
    assert 'id' in data
    assert data['text'] == post_text
    assert 'created_at' in data
    assert data['organization_id'] == registered_org['id']


async def test_create_post_returns_correct_structure(service_client, authenticated_user, registered_org):
    """POST /posts returns a PostDTO with all required fields"""
    response = await service_client.post(
        '/posts',
        json={'text': 'Test post'},
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )

    assert response.status == 201
    data = response.json()
    assert 'id' in data
    assert 'text' in data
    assert 'created_at' in data
    assert 'organization_id' in data


async def test_create_post_persists_in_database(service_client, authenticated_user, registered_org, pgsql):
    """Post is actually saved to database"""
    post_text = f'Persistent post {uuid.uuid4().hex[:6]}'

    response = await service_client.post(
        '/posts',
        json={'text': post_text},
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )
    assert response.status == 201
    post_id = response.json()['id']
    conn = pgsql['postgres-db']
    cursor = conn.cursor()
    cursor.execute(
        'SELECT id, text, organization_id FROM posts WHERE id = %s',
        (post_id,),
    )
    row = cursor.fetchone()
    assert row is not None
    assert row[0] == post_id
    assert row[1] == post_text
    assert row[2] == registered_org['id']


async def test_create_post_multiple_posts(service_client, authenticated_user, registered_org):
    """User can create multiple posts for the same organization"""
    response1 = await service_client.post(
        '/posts',
        json={'text': 'First post'},
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )
    response2 = await service_client.post(
        '/posts',
        json={'text': 'Second post'},
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )

    assert response1.status == 201
    assert response2.status == 201
    assert response1.json()['id'] != response2.json()['id']


async def test_create_post_empty_text_returns_400(service_client, authenticated_user, registered_org):
    """Empty text returns 400 validation error"""
    response = await service_client.post(
        '/posts',
        json={'text': ''},
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )

    assert response.status == 400
    assert response.json()['error']['code'] == 'VALIDATION_ERROR'


async def test_create_post_missing_text_returns_400(service_client, authenticated_user, registered_org):
    """Missing text field returns 400"""
    response = await service_client.post(
        '/posts',
        json={},
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )

    assert response.status == 400
    assert response.json()['error']['code'] == 'MISSING_FIELD'


async def test_create_post_text_too_long_returns_400(service_client, authenticated_user, registered_org):
    """Text longer than 5000 characters returns 400"""
    long_text = 'a' * 5001
    response = await service_client.post(
        '/posts',
        json={'text': long_text},
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )

    assert response.status == 400
    assert response.json()['error']['code'] == 'VALIDATION_ERROR'


async def test_create_post_whitespace_only_text_returns_400(service_client, authenticated_user, registered_org):
    """Whitespace-only text returns 400"""
    response = await service_client.post(
        '/posts',
        json={'text': '   '},
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )

    assert response.status == 400
    assert response.json()['error']['code'] == 'VALIDATION_ERROR'


async def test_create_post_unauthenticated_returns_401(service_client, registered_org):
    """Request without auth token returns 401"""
    response = await service_client.post(
        '/posts',
        json={'text': 'Test post'},
    )

    assert response.status == 401


async def test_create_post_invalid_token_returns_401(service_client, registered_org):
    """Request with invalid token returns 401"""
    response = await service_client.post(
        '/posts',
        json={'text': 'Test post'},
        headers={'Authorization': 'Bearer invalid_token'},
    )

    assert response.status == 401


async def test_create_post_user_without_org_returns_403(service_client, city):
    """User without an organization cannot create posts"""
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

    response = await service_client.post(
        '/posts',
        json={'text': 'Test post'},
        headers={'Authorization': f'Bearer {token}'},
    )

    assert response.status == 403
    assert response.json()['error']['code'] == 'FORBIDDEN'


async def test_create_post_unicode_text(service_client, authenticated_user, registered_org):
    """Post can contain unicode characters"""
    unicode_text = 'Привет мир!'

    response = await service_client.post(
        '/posts',
        json={'text': unicode_text},
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )

    assert response.status == 201
    assert response.json()['text'] == unicode_text


async def test_create_post_with_special_characters(service_client, authenticated_user, registered_org):
    """Post can contain special characters"""
    special_text = 'Test! @#$%^&*()_+{}:"<>?|~`'

    response = await service_client.post(
        '/posts',
        json={'text': special_text},
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )

    assert response.status == 201
    assert response.json()['text'] == special_text


async def test_create_post_creates_created_at_timestamp(service_client, authenticated_user, registered_org):
    """created_at timestamp is set automatically"""
    import time

    before = time.time()
    response = await service_client.post(
        '/posts',
        json={'text': 'Timestamp test'},
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )
    after = time.time()

    assert response.status == 201
    created_at = response.json()['created_at']
    assert created_at is not None
    assert isinstance(created_at, str)


async def test_create_post_json_content_type_required(service_client, authenticated_user, registered_org):
    """Request must have Content-Type: application/json"""
    response = await service_client.post(
        '/posts',
        data='text=plain text',
        headers={
            'Authorization': f"Bearer {authenticated_user['token']}",
            'Content-Type': 'application/x-www-form-urlencoded',
        },
    )
    assert response.status == 400

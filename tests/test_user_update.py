import uuid
import pytest


def make_unique_email(prefix='update'):
    random_part = uuid.uuid4().hex[:10]
    email = f"{prefix}_{random_part}@example.com"
    return email


@pytest.fixture
async def authenticated_user(service_client):
    """Fixture to create and login a user for testing"""
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
    user_data = register_response.json()

    login_response = await service_client.post('/auth/login', json={'email': email, 'password': password})
    assert login_response.status == 200
    tokens = login_response.json()

    result = {
        'id': user_data['id'],
        'email': email,
        'token': tokens['access_token']
    }
    return result


async def test_user_update_success(service_client, authenticated_user):
    """Test updating user successfully"""
    new_email = make_unique_email('updated')
    user_id = authenticated_user['id']
    token = authenticated_user['token']

    response = await service_client.put(
        f"/user/{user_id}",
        json={
            'email': new_email,
            'first_name': 'Updated',
            'last_name': 'Profile',
        },
        headers={'Authorization': f"Bearer {token}"},
    )

    assert response.status == 200
    data = response.json()
    assert data['id'] == user_id
    assert data['email'] == new_email
    assert data['first_name'] == 'Updated'
    assert data['last_name'] == 'Profile'


async def test_user_update_requires_auth(service_client, authenticated_user):
    """Test that update requires authentication"""
    user_id = authenticated_user['id']

    response = await service_client.put(
        f"/user/{user_id}",
        json={
            'email': make_unique_email('updated_no_auth'),
            'first_name': 'Updated',
            'last_name': 'Profile',
        },
    )
    assert response.status == 401


async def test_user_update_other_user_forbidden(service_client, authenticated_user):
    """Test that you can't update other users"""
    other_email = make_unique_email('other')
    other_password = 'TestPassword123'
    other_register = await service_client.post(
        '/user/register',
        json={
            'email': other_email,
            'password': other_password,
            'first_name': 'Other',
            'last_name': 'User',
        },
    )
    assert other_register.status == 201
    other = other_register.json()

    response = await service_client.put(
        f"/user/{other['id']}",
        json={
            'email': make_unique_email('not_allowed'),
            'first_name': 'Not',
            'last_name': 'Allowed',
        },
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )
    assert response.status == 401


async def test_user_update_invalid_path_id(service_client, authenticated_user):
    """Test invalid user id"""
    response = await service_client.put(
        '/user/not-a-number',
        json={
            'email': make_unique_email('bad_id'),
            'first_name': 'Bad',
            'last_name': 'Id',
        },
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )

    assert response.status == 400


async def test_user_update_duplicate_email(service_client, authenticated_user):
    """Test updating to duplicate email"""
    email = make_unique_email('existing')
    password = 'TestPassword123'
    register = await service_client.post(
        '/user/register',
        json={
            'email': existing_email,
            'password': existing_password,
            'first_name': 'Existing',
            'last_name': 'User',
        },
    )
    assert register.status == 201

    response = await service_client.put(
        f"/user/{authenticated_user['id']}",
        json={
            'email': existing_email,
            'first_name': 'Owner',
            'last_name': 'User',
        },
        headers={'Authorization': f"Bearer {authenticated_user['token']}"},
    )

    assert response.status == 409

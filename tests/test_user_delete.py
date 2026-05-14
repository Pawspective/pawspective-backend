import uuid
import pytest


def make_unique_email(prefix='user'):
    random_part = uuid.uuid4().hex[:10]
    return f'{prefix}_{random_part}@example.com'


@pytest.fixture
async def authenticated_user(service_client):
    """Fixture to create and login a user for testing"""
    email = make_unique_email('delete_test')
    password = 'TestPassword123'

    register_response = await service_client.post(
        '/user/register',
        json={
            'email': email,
            'password': password,
            'first_name': 'Delete',
            'last_name': 'User',
        },
    )
    assert register_response.status == 201
    user_data = register_response.json()

    login_response = await service_client.post(
        '/auth/login',
        json={'email': email, 'password': password},
    )
    assert login_response.status == 200
    tokens = login_response.json()

    return {
        'id': user_data['id'],
        'email': email,
        'token': tokens['access_token'],
    }


async def test_user_delete_success(service_client, authenticated_user):
    """Test user can delete their own account"""
    user_id = authenticated_user['id']
    token = authenticated_user['token']

    response = await service_client.delete(
        f'/user/{user_id}',
        headers={'Authorization': f'Bearer {token}'},
    )

    assert response.status == 204


async def test_user_delete_requires_auth(service_client, authenticated_user):
    """Test that delete requires authentication"""
    user_id = authenticated_user['id']

    response = await service_client.delete(f'/user/{user_id}')

    assert response.status == 401


async def test_user_delete_forbidden_for_other_user(service_client, authenticated_user):
    """Test that user cannot delete another user's account"""
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
    other_user = other_register.json()

    response = await service_client.delete(
        f'/user/{other_user["id"]}',
        headers={'Authorization': f'Bearer {authenticated_user["token"]}'},
    )

    assert response.status == 403


async def test_user_delete_nonexistent_user(service_client, authenticated_user):
    """Test deleting a user that doesn't exist"""
    token = authenticated_user['token']
    user_id = authenticated_user['id']

    response1 = await service_client.delete(
        f'/user/{user_id}',
        headers={'Authorization': f'Bearer {token}'},
    )
    assert response1.status == 204

    response2 = await service_client.delete(
        f'/user/{user_id}',
        headers={'Authorization': f'Bearer {token}'},
    )
    assert response2.status == 404


async def test_user_delete_invalid_id(service_client, authenticated_user):
    """Test delete with invalid user id format"""
    token = authenticated_user['token']

    response = await service_client.delete(
        '/user/not-a-number',
        headers={'Authorization': f'Bearer {token}'},
    )

    assert response.status == 400


async def test_user_delete_twice(service_client, authenticated_user):
    """Test deleting already deleted user returns 404"""
    user_id = authenticated_user['id']
    token = authenticated_user['token']

    response1 = await service_client.delete(
        f'/user/{user_id}',
        headers={'Authorization': f'Bearer {token}'},
    )
    assert response1.status == 204

    response2 = await service_client.delete(
        f'/user/{user_id}',
        headers={'Authorization': f'Bearer {token}'},
    )
    assert response2.status == 404

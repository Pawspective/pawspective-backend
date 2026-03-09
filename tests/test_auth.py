import uuid
import pytest


def make_unique_email(prefix='auth'):
    """Helper function to generate unique emails for testing"""
    random_part = uuid.uuid4().hex[:10]
    email = f"{prefix}_{random_part}@example.com"
    return email


@pytest.fixture
async def registered_user(service_client):
    """Fixture that creates a registered user for testing"""
    email = make_unique_email('registered')
    password = 'TestPassword123'

    response = await service_client.post(
        '/user/register',
        json={
            'email': email,
            'password': password,
            'first_name': 'Auth',
            'last_name': 'User',
        },
    )

    assert response.status == 201
    user_data = response.json()

    result = {
        'email': email,
        'password': password,
        'user': user_data
    }
    return result


async def test_auth_login_success(service_client, registered_user):
    """Test that login works correctly"""
    email1 = registered_user['email']
    password1 = registered_user['password']

    response = await service_client.post('/auth/login', json={'email': email1, 'password': password1})

    assert response.status == 200

    data = response.json()
    assert data['token_type'] == 'bearer'
    assert data['access_token']
    assert data['refresh_token']


async def test_auth_refresh_success_and_reuse_fails(service_client, registered_user):
    """Test refresh token functionality"""
    email1 = registered_user['email']
    password1 = registered_user['password']
    login_response = await service_client.post('/auth/login', json={'email': email1, 'password': password1})

    assert login_response.status == 200
    old_tokens = login_response.json()

    refresh_ok = await service_client.post('/auth/refresh', json={'refresh_token': old_tokens['refresh_token']})
    assert refresh_ok.status == 200
    new_tokens = refresh_ok.json()

    assert new_tokens['access_token']
    assert new_tokens['refresh_token']
    assert new_tokens['access_token'] != old_tokens['access_token']
    assert new_tokens['refresh_token'] != old_tokens['refresh_token']

    refresh_reuse = await service_client.post('/auth/refresh', json={'refresh_token': old_tokens['refresh_token']})
    assert refresh_reuse.status == 401


async def test_auth_me_success(service_client, registered_user):
    """Test getting user profile with /auth/me endpoint"""
    email1 = registered_user['email']
    password1 = registered_user['password']
    login_response = await service_client.post('/auth/login', json={'email': email1, 'password': password1})

    assert login_response.status == 200
    tokens = login_response.json()

    access_token1 = tokens['access_token']
    me_response = await service_client.get('/auth/me', headers={'Authorization': f"Bearer {access_token1}"})

    assert me_response.status == 200
    me = me_response.json()
    assert me['id'] == registered_user['user']['id']
    assert me['email'] == registered_user['email']


async def test_auth_me_requires_token(service_client):
    """Test that /auth/me requires authentication"""
    response = await service_client.get('/auth/me')
    assert response.status == 401


async def test_auth_logout_success_revokes_refresh(service_client, registered_user):
    """Test logout functionality"""
    email1 = registered_user['email']
    password1 = registered_user['password']
    login_response = await service_client.post('/auth/login', json={'email': email1, 'password': password1})

    assert login_response.status == 200
    tokens = login_response.json()

    access_token1 = tokens['access_token']
    refresh_token1 = tokens['refresh_token']
    logout_response = await service_client.post(
        '/auth/logout',
        json={'refresh_token': refresh_token1},
        headers={'Authorization': f"Bearer {access_token1}"},
    )

    assert logout_response.status == 200

    refresh_after_logout = await service_client.post('/auth/refresh', json={'refresh_token': refresh_token1})
    assert refresh_after_logout.status == 401


async def test_auth_logout_requires_token(service_client):
    """Test that logout requires token"""
    response = await service_client.post('/auth/logout', json={'refresh_token': 'x'})
    assert response.status == 401


async def test_login_with_wrong_password(service_client, registered_user):
    """Test login with wrong password"""
    email1 = registered_user['email']
    wrong_password = 'WrongPassword123'

    response = await service_client.post(
        '/auth/login',
        json={'email': email1, 'password': wrong_password},
    )

    assert response.status == 401
    payload = response.json()
    assert payload['error']['code'] == 'INVALID_CREDENTIALS'
    assert payload['error']['message'] == 'Invalid email or password'


async def test_login_with_nonexistent_user(service_client):
    """Test login with user that doesn't exist"""
    fake_email = make_unique_email('nonexistent')
    password = 'TestPassword123'

    response = await service_client.post(
        '/auth/login',
        json={'email': fake_email, 'password': password},
    )

    assert response.status == 401
    payload = response.json()
    assert payload['error']['code'] == 'INVALID_CREDENTIALS'
    assert payload['error']['message'] == 'Invalid email or password'


async def test_login_with_empty_email(service_client):
    """Test login with empty email"""
    response = await service_client.post('/auth/login', json={'email': '', 'password': 'TestPassword123'})

    assert response.status == 400
    payload = response.json()
    assert payload['error']['code'] == 'VALIDATION_ERROR'
    assert payload['error']['message'] == 'Validation failed'


async def test_login_with_empty_password(service_client, registered_user):
    """Test login with empty password"""
    email1 = registered_user['email']

    response = await service_client.post('/auth/login', json={'email': email1, 'password': ''})

    assert response.status == 400
    payload = response.json()
    assert payload['error']['code'] == 'VALIDATION_ERROR'
    assert payload['error']['message'] == 'Validation failed'


async def test_login_with_invalid_email_format(service_client):
    """Test login with invalid email format"""
    response = await service_client.post('/auth/login', json={'email': 'not-an-email', 'password': 'TestPassword123'})

    assert response.status == 400
    payload = response.json()
    assert payload['error']['code'] == 'VALIDATION_ERROR'
    assert payload['error']['message'] == 'Validation failed'


async def test_login_with_short_password(service_client):
    """Test login with short password"""
    email1 = make_unique_email('short')
    short_pwd = 'Short1'

    response = await service_client.post('/auth/login', json={'email': email1, 'password': short_pwd})

    assert response.status == 400
    payload = response.json()
    assert payload['error']['code'] == 'VALIDATION_ERROR'
    assert payload['error']['message'] == 'Validation failed'


async def test_refresh_with_invalid_token(service_client):
    """Test refresh with invalid token"""
    bad_token = 'invalid_token_12345'

    response = await service_client.post('/auth/refresh', json={'refresh_token': bad_token})

    assert response.status == 401


async def test_refresh_with_empty_token(service_client):
    """Test refresh with empty token"""
    response = await service_client.post('/auth/refresh', json={'refresh_token': ''})

    assert response.status == 401


async def test_auth_me_with_invalid_token(service_client):
    """Test /auth/me with invalid token"""
    bad_token = 'invalid_token_12345'

    response = await service_client.get('/auth/me', headers={'Authorization': f'Bearer {bad_token}'})

    assert response.status == 401


async def test_auth_me_with_malformed_auth_header(service_client):
    """Test /auth/me with malformed auth header"""
    response = await service_client.get('/auth/me', headers={'Authorization': 'invalid_format'})

    assert response.status == 401


async def test_logout_with_invalid_refresh_token(service_client, registered_user):
    """Test logout with invalid refresh token"""
    email1 = registered_user['email']
    password1 = registered_user['password']
    login_response = await service_client.post(
        '/auth/login', json={'email': email1, 'password': password1}
    )
    assert login_response.status == 200
    tokens = login_response.json()
    access_token1 = tokens['access_token']

    bad_refresh_token = 'invalid_refresh_token'
    response = await service_client.post(
        '/auth/logout',
        json={'refresh_token': bad_refresh_token},
        headers={'Authorization': f"Bearer {access_token1}"},
    )

    assert response.status in [200, 404]

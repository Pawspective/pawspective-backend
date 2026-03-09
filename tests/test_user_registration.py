import uuid


def make_unique_email(prefix='user'):
    random_string = uuid.uuid4().hex[:10]
    result = f"{prefix}_{random_string}@example.com"
    return result


async def test_register_success(service_client):
    """Test successful registration"""
    email = make_unique_email('register')

    response = await service_client.post(
        '/user/register',
        json={
            'email': email,
            'password': 'SecurePassword123',
            'first_name': 'John',
            'last_name': 'Doe',
        },
    )

    assert response.status == 201
    data = response.json()
    assert data['email'] == email
    assert data['first_name'] == 'John'
    assert data['last_name'] == 'Doe'
    assert 'id' in data


async def test_register_duplicate_email(service_client):
    """Test registering with duplicate email"""
    email = make_unique_email('dupe')

    first = await service_client.post(
        '/user/register',
        json={
            'email': email,
            'password': 'SecurePassword123!',
            'first_name': 'John',
            'last_name': 'Doe',
        },
    )
    assert first.status == 201

    second = await service_client.post(
        '/user/register',
        json={
            'email': email,
            'password': 'AnotherPassword123!',
            'first_name': 'Jane',
            'last_name': 'Smith',
        },
    )

    assert second.status == 409
    payload = second.json()
    assert payload["error"]["code"] == 'USER_ALREADY_EXISTS'
    assert payload["error"]["message"] == 'User with this email already exists'


async def test_register_invalid_email(service_client):
    """Test invalid email format"""
    response = await service_client.post(
        '/user/register',
        json={
            'email': 'not-an-email',
            'password': 'SecurePassword123',
            'first_name': 'John',
            'last_name': 'Doe',
        },
    )

    assert response.status == 400
    payload = response.json()
    assert payload["error"]["code"] == 'VALIDATION_ERROR'
    assert payload["error"]["message"] == 'Validation failed'


async def test_register_missing_required_field(service_client):
    """Test missing required field"""
    response = await service_client.post(
        '/user/register',
        json={
            'password': 'SecurePassword123',
            'first_name': 'John',
            'last_name': 'Doe',
        },
    )

    assert response.status == 400
    payload = response.json()
    assert payload["error"]["code"] == 'MISSING_FIELD'
    assert payload["error"]["message"] == 'Missing required field'


async def test_register_invalid_json(service_client):
    """Test invalid JSON"""
    response = await service_client.post(
        '/user/register',
        data='invalid json',
        headers={'Content-Type': 'application/json'},
    )

    assert response.status == 400

import uuid
import pytest


def make_unique_email(prefix='org_upd'):
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
async def another_city(pgsql):
    conn = pgsql['postgres-db']
    cursor = conn.cursor()
    city_name = f'AnotherCity_{uuid.uuid4().hex[:8]}'
    cursor.execute(
        'INSERT INTO cities (name) VALUES (%s) RETURNING id, name',
        (city_name,),
    )
    row = cursor.fetchone()
    return {'id': row[0], 'name': row[1]}


@pytest.fixture
async def org_owner(service_client):
    """Fixture: registered user with an organization"""
    email = make_unique_email('owner')
    password = 'TestPassword123'

    reg = await service_client.post(
        '/user/register',
        json={
            'email': email,
            'password': password,
            'first_name': 'Owner',
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

    return {'token': token}


@pytest.fixture
async def org_with_owner(service_client, org_owner, city):
    """Fixture: user with an organization already created"""
    response = await service_client.post(
        '/orgs',
        json={
            'name': 'Original Org Name',
            'description': 'Original description',
            'city_id': city['id'],
        },
        headers={'Authorization': f"Bearer {org_owner['token']}"},
    )
    assert response.status == 201
    org = response.json()
    return {'org': org, 'token': org_owner['token'], 'city': city}


async def test_update_org_name(service_client, org_with_owner):
    """Test updating only the name field"""
    org_id = org_with_owner['org']['id']
    token = org_with_owner['token']

    response = await service_client.put(
        f'/orgs/{org_id}',
        json={'name': 'Updated Org Name'},
        headers={'Authorization': f'Bearer {token}'},
    )

    assert response.status == 200
    data = response.json()
    assert data['id'] == org_id
    assert data['name'] == 'Updated Org Name'
    assert data['city']['id'] == org_with_owner['city']['id']


async def test_update_org_city(service_client, org_with_owner, another_city):
    """Test updating only the city_id field (regression for issue #115)"""
    org_id = org_with_owner['org']['id']
    token = org_with_owner['token']

    response = await service_client.put(
        f'/orgs/{org_id}',
        json={'city_id': another_city['id']},
        headers={'Authorization': f'Bearer {token}'},
    )

    assert response.status == 200
    data = response.json()
    assert data['id'] == org_id
    assert data['city']['id'] == another_city['id']
    assert data['city']['name'] == another_city['name']


async def test_update_org_description(service_client, org_with_owner):
    """Test updating only the description field"""
    org_id = org_with_owner['org']['id']
    token = org_with_owner['token']

    response = await service_client.put(
        f'/orgs/{org_id}',
        json={'description': 'New description'},
        headers={'Authorization': f'Bearer {token}'},
    )

    assert response.status == 200
    data = response.json()
    assert data['id'] == org_id
    assert data['description'] == 'New description'


async def test_update_org_null_description_is_ignored(service_client, org_with_owner):
    """Test that sending null description leaves it unchanged (null means 'no change')"""
    org_id = org_with_owner['org']['id']
    token = org_with_owner['token']
    original_description = org_with_owner['org'].get('description')

    response = await service_client.put(
        f'/orgs/{org_id}',
        json={'description': None},
        headers={'Authorization': f'Bearer {token}'},
    )

    assert response.status == 200
    data = response.json()
    assert data['id'] == org_id
    assert data.get('description') == original_description


async def test_update_org_all_fields(service_client, org_with_owner, another_city):
    """Test updating all fields at once"""
    org_id = org_with_owner['org']['id']
    token = org_with_owner['token']

    response = await service_client.put(
        f'/orgs/{org_id}',
        json={
            'name': 'Fully Updated Org',
            'description': 'All fields updated',
            'city_id': another_city['id'],
        },
        headers={'Authorization': f'Bearer {token}'},
    )

    assert response.status == 200
    data = response.json()
    assert data['name'] == 'Fully Updated Org'
    assert data['description'] == 'All fields updated'
    assert data['city']['id'] == another_city['id']


async def test_update_org_empty_body_returns_org(service_client, org_with_owner):
    """Test that sending empty body returns current org without SQL error"""
    org_id = org_with_owner['org']['id']
    token = org_with_owner['token']

    response = await service_client.put(
        f'/orgs/{org_id}',
        json={},
        headers={'Authorization': f'Bearer {token}'},
    )

    assert response.status == 200
    data = response.json()
    assert data['id'] == org_id
    assert data['name'] == org_with_owner['org']['name']


async def test_update_org_blank_name_fails(service_client, org_with_owner):
    """Test that blank name is rejected with 400"""
    org_id = org_with_owner['org']['id']
    token = org_with_owner['token']

    response = await service_client.put(
        f'/orgs/{org_id}',
        json={'name': '   '},
        headers={'Authorization': f'Bearer {token}'},
    )

    assert response.status == 400
    payload = response.json()
    assert payload['error']['code'] == 'VALIDATION_ERROR'


async def test_update_org_nonexistent_city_fails(service_client, org_with_owner):
    """Test that a non-existent city_id returns 404"""
    org_id = org_with_owner['org']['id']
    token = org_with_owner['token']

    response = await service_client.put(
        f'/orgs/{org_id}',
        json={'city_id': 999999999},
        headers={'Authorization': f'Bearer {token}'},
    )

    assert response.status == 404
    payload = response.json()
    assert payload['error']['code'] == 'CITY_NOT_FOUND'


async def test_update_org_requires_auth(service_client, org_with_owner):
    """Test that update requires authentication"""
    org_id = org_with_owner['org']['id']

    response = await service_client.put(
        f'/orgs/{org_id}',
        json={'name': 'No Auth Update'},
    )

    assert response.status == 401


async def test_update_org_forbidden_for_non_owner(service_client, org_with_owner):
    """Test that another user cannot update the organization"""
    org_id = org_with_owner['org']['id']

    # Create a second user (without org)
    other_email = make_unique_email('other')
    other_pass = 'TestPassword123'
    await service_client.post(
        '/user/register',
        json={
            'email': other_email,
            'password': other_pass,
            'first_name': 'Other',
            'last_name': 'User',
        },
    )
    login = await service_client.post(
        '/auth/login',
        json={'email': other_email, 'password': other_pass},
    )
    other_token = login.json()['access_token']

    response = await service_client.put(
        f'/orgs/{org_id}',
        json={'name': 'Unauthorized Update'},
        headers={'Authorization': f'Bearer {other_token}'},
    )

    assert response.status == 403
    payload = response.json()
    assert payload['error']['code'] == 'FORBIDDEN'


async def test_update_org_invalid_id(service_client, org_with_owner):
    """Test that non-numeric org id returns 400"""
    token = org_with_owner['token']

    response = await service_client.put(
        '/orgs/not-a-number',
        json={'name': 'Bad ID'},
        headers={'Authorization': f'Bearer {token}'},
    )

    assert response.status == 400


async def test_update_org_nonexistent_org(service_client, org_with_owner):
    """Test updating a non-existent organization returns 403 (forbidden - user has no org with that id)"""
    token = org_with_owner['token']

    response = await service_client.put(
        '/orgs/999999999',
        json={'name': 'Ghost Org'},
        headers={'Authorization': f'Bearer {token}'},
    )

    assert response.status == 403


async def test_update_org_avatar_removes_old_from_s3(
    service_client, org_with_owner, pgsql, testpoint
):
    """Replacing avatar_url triggers S3 DELETE for the old one"""
    org_id = org_with_owner['org']['id']
    token = org_with_owner['token']
    old_avatar = f'{uuid.uuid4().hex}.jpg'
    new_avatar = f'{uuid.uuid4().hex}.jpg'

    conn = pgsql['postgres-db']
    cursor = conn.cursor()
    cursor.execute(
        'UPDATE organizations SET avatar_url = %s WHERE id = %s', (old_avatar, org_id))

    keys_deleted = []

    @testpoint('s3-delete-file')
    def s3_handler(data):
        keys_deleted.append(data['key'])

    response = await service_client.put(
        f'/orgs/{org_id}',
        json={'avatar_url': new_avatar},
        headers={'Authorization': f'Bearer {token}'},
    )

    assert response.status == 200
    assert s3_handler.times_called == 1
    assert old_avatar.split('/')[-1] in keys_deleted[0]


async def test_update_org_same_avatar_no_s3_delete(
    service_client, org_with_owner, pgsql, mockserver
):
    """Sending the same avatar_url does not trigger S3 DELETE"""
    org_id = org_with_owner['org']['id']
    token = org_with_owner['token']
    avatar = f'{uuid.uuid4().hex}.jpg'

    conn = pgsql['postgres-db']
    cursor = conn.cursor()
    cursor.execute(
        'UPDATE organizations SET avatar_url = %s WHERE id = %s', (avatar, org_id))

    s3_deleted = []

    @mockserver.handler('/photos/', prefix=True)
    def s3_handler(request):
        if request.method == 'DELETE':
            s3_deleted.append(request.path)
        return mockserver.make_response('', 204)

    response = await service_client.put(
        f'/orgs/{org_id}',
        json={'avatar_url': avatar},
        headers={'Authorization': f'Bearer {token}'},
    )

    assert response.status == 200
    assert len(s3_deleted) == 0


async def test_update_org_without_avatar_field_no_s3_delete(
    service_client, org_with_owner, pgsql, mockserver
):
    """Omitting avatar_url in update does not trigger S3 DELETE"""
    org_id = org_with_owner['org']['id']
    token = org_with_owner['token']
    avatar = f'{uuid.uuid4().hex}.jpg'

    conn = pgsql['postgres-db']
    cursor = conn.cursor()
    cursor.execute(
        'UPDATE organizations SET avatar_url = %s WHERE id = %s', (avatar, org_id))

    s3_deleted = []

    @mockserver.handler('/photos/', prefix=True)
    def s3_handler(request):
        if request.method == 'DELETE':
            s3_deleted.append(request.path)
        return mockserver.make_response('', 204)

    response = await service_client.put(
        f'/orgs/{org_id}',
        json={'name': 'Name only update'},
        headers={'Authorization': f'Bearer {token}'},
    )

    assert response.status == 200
    assert len(s3_deleted) == 0

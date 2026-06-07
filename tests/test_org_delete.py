import uuid
import pytest

S3_PUBLIC_BASE = 'https://hollow1crown.storage.yandexcloud.net'


def make_unique_email(prefix='org_delete'):
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
async def org_owner(service_client):
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

    return {'token': token, 'email': email, 'password': password}


@pytest.fixture
async def org_with_owner(service_client, org_owner, city):
    response = await service_client.post(
        '/orgs',
        json={
            'name': 'Test Organization',
            'description': 'Test description',
            'city_id': city['id'],
        },
        headers={'Authorization': f"Bearer {org_owner['token']}"},
    )
    assert response.status == 201
    org = response.json()
    return {
        'org': org,
        'token': org_owner['token'],
        'city': city,
        'owner_email': org_owner['email'],
    }


async def test_org_delete_success(service_client, org_with_owner):
    org_id = org_with_owner['org']['id']
    token = org_with_owner['token']

    response = await service_client.delete(
        f'/orgs/{org_id}',
        headers={'Authorization': f'Bearer {token}'},
    )

    assert response.status == 204

    get_response = await service_client.get(f'/orgs/{org_id}')
    assert get_response.status == 404


async def test_org_delete_requires_auth(service_client, org_with_owner):
    org_id = org_with_owner['org']['id']

    response = await service_client.delete(f'/orgs/{org_id}')

    assert response.status == 401


async def test_org_delete_forbidden_for_non_owner(service_client, org_with_owner):
    org_id = org_with_owner['org']['id']

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

    response = await service_client.delete(
        f'/orgs/{org_id}',
        headers={'Authorization': f'Bearer {other_token}'},
    )

    assert response.status == 403


async def test_org_delete_nonexistent_org(service_client, org_with_owner):
    token = org_with_owner['token']

    response = await service_client.delete(
        '/orgs/999999999',
        headers={'Authorization': f'Bearer {token}'},
    )

    assert response.status == 403


async def test_org_delete_invalid_id(service_client, org_with_owner):
    token = org_with_owner['token']

    response = await service_client.delete(
        '/orgs/not-a-number',
        headers={'Authorization': f'Bearer {token}'},
    )

    assert response.status == 400


async def test_org_delete_twice(service_client, org_with_owner):
    org_id = org_with_owner['org']['id']
    token = org_with_owner['token']

    response1 = await service_client.delete(
        f'/orgs/{org_id}',
        headers={'Authorization': f'Bearer {token}'},
    )
    assert response1.status == 204

    response2 = await service_client.delete(
        f'/orgs/{org_id}',
        headers={'Authorization': f'Bearer {token}'},
    )
    assert response2.status == 403


async def test_org_delete_with_avatar_cleans_s3(
    service_client, org_with_owner, pgsql, testpoint
):
    """Deleting an org with an avatar triggers S3 DELETE for the avatar"""
    org_id = org_with_owner['org']['id']
    token = org_with_owner['token']
    avatar_url = f'{S3_PUBLIC_BASE}/photos/{uuid.uuid4().hex}.jpg'

    conn = pgsql['postgres-db']
    cursor = conn.cursor()
    cursor.execute(
        'UPDATE organizations SET avatar_url = %s WHERE id = %s', (avatar_url, org_id))

    keys_deleted = []

    @testpoint('s3-delete-file')
    def s3_handler(data):
        keys_deleted.append(data['key'])

    response = await service_client.delete(
        f'/orgs/{org_id}',
        headers={'Authorization': f'Bearer {token}'},
    )

    assert response.status == 204
    assert s3_handler.times_called == 1
    assert avatar_url.split('/')[-1] in keys_deleted[0]


async def test_org_delete_without_avatar_no_s3_call(
    service_client, org_with_owner, mockserver
):
    """Deleting an org with no avatar makes no S3 calls"""
    org_id = org_with_owner['org']['id']
    token = org_with_owner['token']

    s3_deleted = []

    @mockserver.handler('/photos/', prefix=True)
    def s3_handler(request):
        if request.method == 'DELETE':
            s3_deleted.append(request.path)
        return mockserver.make_response('', 204)

    response = await service_client.delete(
        f'/orgs/{org_id}',
        headers={'Authorization': f'Bearer {token}'},
    )

    assert response.status == 204
    assert len(s3_deleted) == 0

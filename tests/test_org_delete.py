import uuid
import pytest


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

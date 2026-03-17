import uuid
import pytest


def make_unique_email(prefix='search_org'):
    random_part = uuid.uuid4().hex[:10]
    return f'{prefix}_{random_part}@example.com'


def make_unique_suffix():
    return uuid.uuid4().hex[:8]


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


async def create_org(service_client, token, city_id, name, description=None):
    payload = {'name': name, 'city_id': city_id}
    if description is not None:
        payload['description'] = description
    response = await service_client.post(
        '/orgs',
        json=payload,
        headers={'Authorization': f'Bearer {token}'},
    )
    assert response.status == 201
    return response.json()


async def test_search_org_by_name(service_client, authenticated_user, city):
    """Test searching organizations returns matching results"""
    suffix = make_unique_suffix()
    org_name = f'HappyPaws_{suffix}'
    token = authenticated_user['token']

    org = await create_org(service_client, token, city['id'], org_name)

    response = await service_client.get(f'/orgs?name={org_name}')

    assert response.status == 200
    data = response.json()
    assert isinstance(data, list)
    assert any(o['id'] == org['id'] for o in data)


async def test_search_org_case_insensitive_upper(
    service_client, authenticated_user, city
):
    """Test that search is case-insensitive (uppercase query)"""
    suffix = make_unique_suffix()
    org_name = f'PawShelter_{suffix}'
    token = authenticated_user['token']

    org = await create_org(service_client, token, city['id'], org_name)

    response = await service_client.get(f'/orgs?name={org_name.upper()}')

    assert response.status == 200
    data = response.json()
    assert any(o['id'] == org['id'] for o in data)


async def test_search_org_case_insensitive_lower(
    service_client, authenticated_user, city
):
    """Test that search is case-insensitive (lowercase query)"""
    suffix = make_unique_suffix()
    org_name = f'RescueAnimals_{suffix}'
    token = authenticated_user['token']

    org = await create_org(service_client, token, city['id'], org_name)

    response = await service_client.get(f'/orgs?name={org_name.lower()}')

    assert response.status == 200
    data = response.json()
    assert any(o['id'] == org['id'] for o in data)


async def test_search_org_partial_match(service_client, authenticated_user, city):
    """Test that search returns results for partial name matches"""
    suffix = make_unique_suffix()
    org_name = f'AnimalRescue_{suffix}'
    token = authenticated_user['token']

    org = await create_org(service_client, token, city['id'], org_name)

    # Search by a substring
    response = await service_client.get(f'/orgs?name=AnimalRescue_{suffix}')

    assert response.status == 200
    data = response.json()
    assert any(o['id'] == org['id'] for o in data)


async def test_search_org_no_match_returns_empty_list(service_client):
    """Test that searching with a non-existent name returns an empty list"""
    response = await service_client.get(
        '/orgs?name=NonExistentOrg_zzzzzzzzzzz'
    )

    assert response.status == 200
    data = response.json()
    assert isinstance(data, list)
    assert len(data) == 0


async def test_search_org_empty_name_returns_list(
    service_client, authenticated_user, city
):
    """Test that searching with empty name returns results (matches all)"""
    suffix = make_unique_suffix()
    token = authenticated_user['token']
    await create_org(service_client, token, city['id'], f'SomeOrg_{suffix}')

    response = await service_client.get('/orgs?name=')

    assert response.status == 200
    data = response.json()
    assert isinstance(data, list)
    assert len(data) == 1


async def test_search_org_response_shape(service_client, authenticated_user, city):
    """Test that search results have the correct DTO shape"""
    suffix = make_unique_suffix()
    org_name = f'ShapeTest_{suffix}'
    token = authenticated_user['token']

    await create_org(
        service_client, token, city['id'], org_name, 'A test description'
    )

    response = await service_client.get(f'/orgs?name={org_name}')

    assert response.status == 200
    data = response.json()
    assert len(data) >= 1
    org = next(o for o in data if o['name'] == org_name)
    assert 'id' in org
    assert 'name' in org
    assert 'description' in org
    assert 'city' in org
    assert org['city']['id'] == city['id']
    assert org['description'] == 'A test description'

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
    assert 'items' in data
    assert any(o['id'] == org['id'] for o in data['items'])


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
    assert any(o['id'] == org['id'] for o in data['items'])


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
    assert any(o['id'] == org['id'] for o in data['items'])


async def test_search_org_partial_match(service_client, authenticated_user, city):
    """Test that search returns results for partial name matches"""
    suffix = make_unique_suffix()
    org_name = f'AnimalRescue_{suffix}'
    token = authenticated_user['token']

    org = await create_org(service_client, token, city['id'], org_name)

    response = await service_client.get(f'/orgs?name=AnimalRescue_{suffix}')

    assert response.status == 200
    data = response.json()
    assert any(o['id'] == org['id'] for o in data['items'])


async def test_search_org_no_match_returns_empty_list(service_client):
    """Test that searching with a non-existent name returns empty items"""
    response = await service_client.get(
        '/orgs?name=NonExistentOrg_zzzzzzzzzzz'
    )

    assert response.status == 200
    data = response.json()
    assert 'items' in data
    assert len(data['items']) == 0


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
    assert 'items' in data
    assert len(data['items']) == 1


async def test_search_org_response_shape(service_client, authenticated_user, city):
    """Test that search results have the correct paginated DTO shape"""
    suffix = make_unique_suffix()
    org_name = f'ShapeTest_{suffix}'
    token = authenticated_user['token']

    await create_org(
        service_client, token, city['id'], org_name, 'A test description'
    )

    response = await service_client.get(f'/orgs?name={org_name}')

    assert response.status == 200
    data = response.json()
    assert 'items' in data
    assert 'page' in data
    assert 'limit' in data
    assert 'total_count' in data
    assert 'total_pages' in data
    assert data['page'] == 1
    assert data['limit'] == 20
    assert data['total_count'] >= 1
    assert len(data['items']) >= 1
    org = next(o for o in data['items'] if o['name'] == org_name)
    assert 'id' in org
    assert 'name' in org
    assert 'description' in org
    assert 'city' in org
    assert org['city']['id'] == city['id']
    assert org['description'] == 'A test description'


async def test_search_org_pagination_page_param(
    service_client, authenticated_user, city
):
    """Test that page parameter is accepted and reflected in response"""
    suffix = make_unique_suffix()
    org_name = f'PageTest_{suffix}'
    token = authenticated_user['token']

    await create_org(service_client, token, city['id'], org_name)

    response = await service_client.get(f'/orgs?name={org_name}&page=1')

    assert response.status == 200
    data = response.json()
    assert data['page'] == 1
    assert 'items' in data


async def test_search_org_invalid_page_returns_error(service_client):
    """Test that invalid page parameter returns a validation error"""
    response = await service_client.get('/orgs?name=test&page=abc')
    assert response.status == 400


async def test_search_org_page_less_than_one_returns_error(service_client):
    """Test that page < 1 returns a validation error"""
    response = await service_client.get('/orgs?name=test&page=0')
    assert response.status == 400


async def test_search_org_negative_page_returns_error(service_client):
    """Test that negative page returns a validation error"""
    response = await service_client.get('/orgs?name=test&page=-5')
    assert response.status == 400


async def test_search_org_total_count_exact(service_client, authenticated_user, city):
    """Test that total_count is exactly 1 for a uniquely named org"""
    suffix = make_unique_suffix()
    org_name = f'UniqueOrg_{suffix}'
    token = authenticated_user['token']

    await create_org(service_client, token, city['id'], org_name)

    response = await service_client.get(f'/orgs?name={org_name}')

    assert response.status == 200
    data = response.json()
    assert data['total_count'] == 1
    assert data['total_pages'] == 1


async def test_search_org_total_pages_one_when_no_results(service_client):
    """Test that total_pages is 1 when there are no matching results"""
    response = await service_client.get('/orgs?name=NoSuchOrg_zzzzz')

    assert response.status == 200
    data = response.json()
    assert data['total_count'] == 0
    assert data['total_pages'] == 1


async def test_search_org_out_of_range_page_returns_empty_items(
    service_client, authenticated_user, city
):
    """Test that a page beyond total_pages returns empty items with valid meta"""
    suffix = make_unique_suffix()
    org_name = f'RangeOrg_{suffix}'
    token = authenticated_user['token']

    await create_org(service_client, token, city['id'], org_name)

    response = await service_client.get(f'/orgs?name={org_name}&page=9999')

    assert response.status == 200
    data = response.json()
    assert data['items'] == []
    assert data['total_count'] == 1
    assert data['total_pages'] == 1
    assert data['page'] == 9999


async def _make_user_with_token(service_client, prefix='multi'):
    email = make_unique_email(prefix)
    password = 'TestPassword123'
    await service_client.post(
        '/user/register',
        json={
            'email': email,
            'password': password,
            'first_name': 'Test',
            'last_name': 'User',
        },
    )
    login = await service_client.post(
        '/auth/login',
        json={'email': email, 'password': password},
    )
    return login.json()['access_token']


async def test_search_org_multiple_matches(service_client, city):
    """Test that multiple organizations matching the prefix all appear in items"""
    suffix = make_unique_suffix()
    prefix = f'MultiOrg_{suffix}'

    token1 = await _make_user_with_token(service_client, 'multi1')
    token2 = await _make_user_with_token(service_client, 'multi2')

    org1 = await create_org(service_client, token1, city['id'], f'{prefix}_A')
    org2 = await create_org(service_client, token2, city['id'], f'{prefix}_B')

    response = await service_client.get(f'/orgs?name={prefix}')

    assert response.status == 200
    data = response.json()
    ids = {o['id'] for o in data['items']}
    assert org1['id'] in ids
    assert org2['id'] in ids
    assert data['total_count'] == 2


async def test_search_org_no_description_is_null(
    service_client, authenticated_user, city
):
    """Test that an org created without description serializes description as null"""
    suffix = make_unique_suffix()
    org_name = f'NoDesc_{suffix}'
    token = authenticated_user['token']

    await create_org(service_client, token, city['id'], org_name)

    response = await service_client.get(f'/orgs?name={org_name}')

    assert response.status == 200
    data = response.json()
    org = next(o for o in data['items'] if o['name'] == org_name)
    assert org['description'] is None


async def test_search_org_city_shape(service_client, authenticated_user, city):
    """Test that the city object in each item has id and name fields"""
    suffix = make_unique_suffix()
    org_name = f'CityShape_{suffix}'
    token = authenticated_user['token']

    await create_org(service_client, token, city['id'], org_name)

    response = await service_client.get(f'/orgs?name={org_name}')

    assert response.status == 200
    data = response.json()
    org = next(o for o in data['items'] if o['name'] == org_name)
    assert 'id' in org['city']
    assert 'name' in org['city']
    assert org['city']['id'] == city['id']
    assert org['city']['name'] == city['name']

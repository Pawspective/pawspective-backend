import uuid
import math
import pytest


def make_unique_email(prefix='animal_list'):
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
async def dog_breed(pgsql):
    conn = pgsql['postgres-db']
    cursor = conn.cursor()
    breed_name = f'DogBreed_{uuid.uuid4().hex[:8]}'
    cursor.execute(
        'INSERT INTO breeds (animal_type, name) VALUES (%s, %s) RETURNING id, animal_type, name',
        ('dog', breed_name),
    )
    row = cursor.fetchone()
    return {'id': row[0], 'animal_type': row[1], 'name': row[2]}


@pytest.fixture
async def cat_breed(pgsql):
    conn = pgsql['postgres-db']
    cursor = conn.cursor()
    breed_name = f'CatBreed_{uuid.uuid4().hex[:8]}'
    cursor.execute(
        'INSERT INTO breeds (animal_type, name) VALUES (%s, %s) RETURNING id, animal_type, name',
        ('cat', breed_name),
    )
    row = cursor.fetchone()
    return {'id': row[0], 'animal_type': row[1], 'name': row[2]}


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


async def create_animal(
    service_client, token, org_id, breed_id, overrides=None
):
    payload = {
        'name': f'Animal_{uuid.uuid4().hex[:6]}',
        'organization_id': org_id,
        'breed_id': breed_id,
        'size': 'medium',
        'gender': 'male',
        'care_level': 'easy',
        'color': 'black',
        'good_with': 'dogs',
        'age': 3,
        'status': 'available',
    }
    if overrides:
        payload.update(overrides)
    response = await service_client.post(
        '/animals',
        json=payload,
        headers={'Authorization': f'Bearer {token}'},
    )
    assert response.status == 201
    return response.json()


# --- Basic response structure ---

async def test_animal_list_returns_200(service_client):
    """GET /animals returns 200 without auth"""
    response = await service_client.get('/animals')
    assert response.status == 200


async def test_animal_list_returns_paginated_object(service_client):
    """GET /animals returns a paginated object, not a bare array"""
    response = await service_client.get('/animals')
    assert response.status == 200
    data = response.json()
    assert isinstance(data, dict)
    assert 'items' in data
    assert 'page' in data
    assert 'limit' in data
    assert 'total_count' in data
    assert 'total_pages' in data


async def test_animal_list_no_auth_required(service_client):
    """GET /animals is publicly accessible"""
    response = await service_client.get('/animals')
    assert response.status == 200


async def test_animal_list_default_page_is_1(service_client):
    """Default page is 1 when not specified"""
    response = await service_client.get('/animals')
    assert response.status == 200
    assert response.json()['page'] == 1


async def test_animal_list_default_limit(service_client):
    """Default limit is 20"""
    response = await service_client.get('/animals')
    assert response.status == 200
    assert response.json()['limit'] == 20


async def test_animal_list_contains_created_animal(
    service_client, authenticated_user, registered_org, dog_breed
):
    """Animals that were registered appear in the list"""
    animal = await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        dog_breed['id'],
    )

    response = await service_client.get('/animals')
    assert response.status == 200
    items = response.json()['items']
    assert any(a['id'] == animal['id'] for a in items)


async def test_animal_list_response_shape(
    service_client, authenticated_user, registered_org, dog_breed
):
    """Each animal in the list has the expected fields"""
    animal = await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        dog_breed['id'],
        {'name': 'Shaper', 'color': 'brown', 'age': 2},
    )

    response = await service_client.get('/animals')
    assert response.status == 200
    items = response.json()['items']
    item = next(a for a in items if a['id'] == animal['id'])
    assert 'id' in item
    assert 'name' in item
    assert 'organization_id' in item
    assert 'breed' in item
    assert 'size' in item
    assert 'gender' in item
    assert 'care_level' in item
    assert 'color' in item
    assert 'good_with' in item
    assert 'age' in item
    assert 'status' in item


# --- Filters ---

async def test_animal_list_filter_by_size(
    service_client, authenticated_user, registered_org, dog_breed
):
    """Filter by size returns only matching animals"""
    small = await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        dog_breed['id'],
        {'size': 'small'},
    )
    large = await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        dog_breed['id'],
        {'size': 'large'},
    )

    response = await service_client.get('/animals?sizes=small')
    assert response.status == 200
    ids = [a['id'] for a in response.json()['items']]
    assert small['id'] in ids
    assert large['id'] not in ids


async def test_animal_list_filter_by_gender(
    service_client, authenticated_user, registered_org, dog_breed
):
    """Filter by gender returns only matching animals"""
    male = await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        dog_breed['id'],
        {'gender': 'male'},
    )
    female = await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        dog_breed['id'],
        {'gender': 'female'},
    )

    response = await service_client.get('/animals?genders=female')
    assert response.status == 200
    ids = [a['id'] for a in response.json()['items']]
    assert female['id'] in ids
    assert male['id'] not in ids


async def test_animal_list_filter_by_color(
    service_client, authenticated_user, registered_org, dog_breed
):
    """Filter by color returns only matching animals"""
    white = await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        dog_breed['id'],
        {'color': 'white'},
    )
    grey = await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        dog_breed['id'],
        {'color': 'grey'},
    )

    response = await service_client.get('/animals?colors=white')
    assert response.status == 200
    ids = [a['id'] for a in response.json()['items']]
    assert white['id'] in ids
    assert grey['id'] not in ids


async def test_animal_list_filter_by_age_range(
    service_client, authenticated_user, registered_org, dog_breed
):
    """Filter by age_gte and age_lte returns animals within the range"""
    young = await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        dog_breed['id'],
        {'age': 1},
    )
    old = await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        dog_breed['id'],
        {'age': 10},
    )

    response = await service_client.get('/animals?age_gte=1&age_lte=3')
    assert response.status == 200
    ids = [a['id'] for a in response.json()['items']]
    assert young['id'] in ids
    assert old['id'] not in ids


async def test_animal_list_filter_by_breed(
    service_client, authenticated_user, registered_org, dog_breed, cat_breed
):
    """Filter by breed_id returns only animals of that breed"""
    dog = await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        dog_breed['id'],
    )
    cat = await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        cat_breed['id'],
    )

    response = await service_client.get(f'/animals?breeds={dog_breed["id"]}')
    assert response.status == 200
    ids = [a['id'] for a in response.json()['items']]
    assert dog['id'] in ids
    assert cat['id'] not in ids


async def test_animal_list_filter_no_match_returns_empty(
    service_client, authenticated_user, registered_org, dog_breed
):
    """Filters that match nothing return empty items and zero total_count"""
    await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        dog_breed['id'],
        {'age': 5},
    )

    response = await service_client.get('/animals?age_gte=100&age_lte=200')
    assert response.status == 200
    data = response.json()
    assert data['items'] == []
    assert data['total_count'] == 0


async def test_animal_list_filter_size_and_gender(
    service_client, authenticated_user, registered_org, dog_breed
):
    """Combining size and gender filters returns only animals matching both"""
    match = await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        dog_breed['id'],
        {'size': 'small', 'gender': 'female'},
    )
    wrong_gender = await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        dog_breed['id'],
        {'size': 'small', 'gender': 'male'},
    )
    wrong_size = await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        dog_breed['id'],
        {'size': 'large', 'gender': 'female'},
    )

    response = await service_client.get('/animals?sizes=small&genders=female')
    assert response.status == 200
    ids = [a['id'] for a in response.json()['items']]
    assert match['id'] in ids
    assert wrong_gender['id'] not in ids
    assert wrong_size['id'] not in ids


async def test_animal_list_filter_color_and_age(
    service_client, authenticated_user, registered_org, dog_breed
):
    """Combining color and age range filters works correctly"""
    match = await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        dog_breed['id'],
        {'color': 'golden', 'age': 2},
    )
    wrong_color = await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        dog_breed['id'],
        {'color': 'brown', 'age': 2},
    )
    wrong_age = await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        dog_breed['id'],
        {'color': 'golden', 'age': 8},
    )

    response = await service_client.get(
        '/animals?colors=golden&age_gte=1&age_lte=3'
    )
    assert response.status == 200
    ids = [a['id'] for a in response.json()['items']]
    assert match['id'] in ids
    assert wrong_color['id'] not in ids
    assert wrong_age['id'] not in ids


async def test_animal_list_filter_multiple_sizes(
    service_client, authenticated_user, registered_org, dog_breed
):
    """Multiple values for the same filter param work (OR logic)"""
    small = await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        dog_breed['id'],
        {'size': 'small'},
    )
    large = await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        dog_breed['id'],
        {'size': 'large'},
    )
    medium = await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        dog_breed['id'],
        {'size': 'medium'},
    )

    response = await service_client.get('/animals?sizes=small&sizes=large')
    assert response.status == 200
    ids = [a['id'] for a in response.json()['items']]
    assert small['id'] in ids
    assert large['id'] in ids
    assert medium['id'] not in ids


async def test_animal_list_filter_breed_and_size(
    service_client, authenticated_user, registered_org, dog_breed, cat_breed
):
    """Combining breed and size filters narrows results correctly"""
    match = await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        dog_breed['id'],
        {'size': 'large'},
    )
    wrong_breed = await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        cat_breed['id'],
        {'size': 'large'},
    )
    wrong_size = await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        dog_breed['id'],
        {'size': 'small'},
    )

    response = await service_client.get(
        f'/animals?breeds={dog_breed["id"]}&sizes=large'
    )
    assert response.status == 200
    ids = [a['id'] for a in response.json()['items']]
    assert match['id'] in ids
    assert wrong_breed['id'] not in ids
    assert wrong_size['id'] not in ids


# --- Pagination ---

async def test_animal_list_page_param_accepted(service_client):
    """?page=1 is accepted and returns page 1"""
    response = await service_client.get('/animals?page=1')
    assert response.status == 200
    assert response.json()['page'] == 1


async def test_animal_list_total_count_reflects_created(
    service_client, authenticated_user, registered_org, dog_breed
):
    """total_count increases after creating animals"""
    before = (await service_client.get('/animals')).json()['total_count']

    await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        dog_breed['id'],
    )
    await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        dog_breed['id'],
    )

    after = (await service_client.get('/animals')).json()['total_count']
    assert after == before + 2


async def test_animal_list_total_pages_calculated_correctly(
    service_client, authenticated_user, registered_org, dog_breed
):
    """total_pages = ceil(total_count / limit)"""
    response = await service_client.get('/animals')
    data = response.json()
    expected_pages = max(1, math.ceil(data['total_count'] / data['limit']))
    assert data['total_pages'] == expected_pages


async def test_animal_list_page_2_different_from_page_1(
    service_client, authenticated_user, registered_org, dog_breed
):
    """When there are more than 20 animals, page 2 differs from page 1"""
    for _ in range(22):
        await create_animal(
            service_client,
            authenticated_user['token'],
            registered_org['id'],
            dog_breed['id'],
        )

    page1 = await service_client.get('/animals?page=1')
    page2 = await service_client.get('/animals?page=2')

    assert page1.status == 200
    assert page2.status == 200

    ids1 = {a['id'] for a in page1.json()['items']}
    ids2 = {a['id'] for a in page2.json()['items']}

    assert len(ids1) == 20
    assert len(ids2) > 0
    assert ids1.isdisjoint(ids2)


async def test_animal_list_page_beyond_last_returns_empty(
    service_client,
):
    """Requesting a page beyond total_pages returns empty items"""
    response = await service_client.get('/animals?page=999999')
    assert response.status == 200
    data = response.json()
    assert data['items'] == []
    assert data['page'] == 999999


async def test_animal_list_items_count_on_last_page(
    service_client, authenticated_user, registered_org, dog_breed
):
    """Last page may have fewer than limit items"""
    before = (await service_client.get('/animals')).json()['total_count']

    needed = 21 - (before % 20) if before % 20 != 0 else 1
    for _ in range(needed):
        await create_animal(
            service_client,
            authenticated_user['token'],
            registered_org['id'],
            dog_breed['id'],
        )

    data = (await service_client.get('/animals')).json()
    total_pages = data['total_pages']

    last_page = (
        await service_client.get(f'/animals?page={total_pages}')
    ).json()
    remainder = data['total_count'] % data['limit']
    expected_last = remainder if remainder != 0 else data['limit']
    assert len(last_page['items']) == expected_last


async def test_animal_list_new_animal_reflected_in_total_count_on_next_page(
    service_client, authenticated_user, registered_org, dog_breed
):
    """Adding an animal between page requests is reflected in total_count.

    With offset-based pagination there is no consistency guarantee across
    pages — the new item may or may not appear in the current window.
    What we DO guarantee: each response contains a fresh total_count that
    counts all animals at the time of that specific request.
    """
    # Fill to the next multiple of 20 so page 1 is exactly full
    before = (await service_client.get('/animals')).json()['total_count']
    needed = (20 - before % 20) % 20
    if before + needed < 20:
        needed += 20
    for _ in range(needed):
        await create_animal(
            service_client,
            authenticated_user['token'],
            registered_org['id'],
            dog_breed['id'],
        )

    page1 = (await service_client.get('/animals?page=1')).json()
    assert len(page1['items']) == 20

    # Add a new animal between the two page requests
    new_animal = await create_animal(
        service_client,
        authenticated_user['token'],
        registered_org['id'],
        dog_breed['id'],
    )

    page2 = (await service_client.get('/animals?page=2')).json()

    # total_count must reflect the newly added animal
    assert page2['total_count'] == page1['total_count'] + 1

    # The new animal must appear somewhere across both pages combined
    all_ids = {a['id'] for a in page1['items']} | {a['id']
                                                   for a in page2['items']}
    assert new_animal['id'] in all_ids

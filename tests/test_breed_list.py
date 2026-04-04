import uuid
import pytest


@pytest.fixture
async def dog_breed(pgsql):
    conn = pgsql['postgres-db']
    cursor = conn.cursor()
    name = f'Labrador_{uuid.uuid4().hex[:8]}'
    cursor.execute(
        "INSERT INTO breeds (animal_type, name) VALUES ('dog', %s) RETURNING id, animal_type, name",
        (name,),
    )
    row = cursor.fetchone()
    return {'id': row[0], 'animal_type': row[1], 'name': row[2]}


@pytest.fixture
async def cat_breed(pgsql):
    conn = pgsql['postgres-db']
    cursor = conn.cursor()
    name = f'Siamese_{uuid.uuid4().hex[:8]}'
    cursor.execute(
        "INSERT INTO breeds (animal_type, name) VALUES ('cat', %s) RETURNING id, animal_type, name",
        (name,),
    )
    row = cursor.fetchone()
    return {'id': row[0], 'animal_type': row[1], 'name': row[2]}


async def test_breed_list_by_dog_type(service_client, dog_breed, cat_breed):
    """Test that GET /breeds?type=dog returns only dog breeds"""
    response = await service_client.get('/breeds?type=dog')

    assert response.status == 200
    data = response.json()
    assert isinstance(data, list)
    assert any(b['id'] == dog_breed['id'] for b in data)
    assert not any(b['id'] == cat_breed['id'] for b in data)


async def test_breed_list_by_cat_type(service_client, dog_breed, cat_breed):
    """Test that GET /breeds?type=cat returns only cat breeds"""
    response = await service_client.get('/breeds?type=cat')

    assert response.status == 200
    data = response.json()
    assert isinstance(data, list)
    assert any(b['id'] == cat_breed['id'] for b in data)
    assert not any(b['id'] == dog_breed['id'] for b in data)


async def test_breed_list_response_shape(service_client, dog_breed):
    """Test that breed list items have the correct fields"""
    response = await service_client.get('/breeds?type=dog')

    assert response.status == 200
    data = response.json()
    assert len(data) >= 1
    breed = next(b for b in data if b['id'] == dog_breed['id'])
    assert 'id' in breed
    assert 'name' in breed
    assert 'animal_type' in breed
    assert breed['animal_type'] == 'dog'
    assert breed['name'] == dog_breed['name']


async def test_breed_list_missing_type_returns_400(service_client):
    """Test that missing type query parameter returns 400"""
    response = await service_client.get('/breeds')

    assert response.status == 400


async def test_breed_list_invalid_type_returns_400(service_client):
    """Test that an invalid type value returns 400"""
    response = await service_client.get('/breeds?type=fish')

    assert response.status == 400


async def test_breed_list_no_auth_required(service_client, dog_breed):
    """Test that GET /breeds does not require authentication"""
    response = await service_client.get('/breeds?type=dog')

    assert response.status == 200


async def test_breed_list_empty_result_for_other_type(service_client):
    """Test that type=other returns an empty or valid list (no error)"""
    response = await service_client.get('/breeds?type=other')

    assert response.status == 200
    data = response.json()
    assert isinstance(data, list)

import uuid
import pytest


@pytest.fixture
async def cities(pgsql):
    conn = pgsql['postgres-db']
    cursor = conn.cursor()
    names = [f'City_{uuid.uuid4().hex[:8]}' for _ in range(3)]
    ids = []
    for name in names:
        cursor.execute(
            'INSERT INTO cities (name) VALUES (%s) RETURNING id',
            (name,),
        )
        ids.append(cursor.fetchone()[0])
    return [{'id': id_, 'name': name} for id_, name in zip(ids, names)]


async def test_city_list_returns_200(service_client):
    """Test that GET /city returns 200"""
    response = await service_client.get('/city')
    assert response.status == 200


async def test_city_list_returns_array(service_client):
    """Test that GET /city returns a JSON array"""
    response = await service_client.get('/city')
    assert response.status == 200
    data = response.json()
    assert isinstance(data, list)


async def test_city_list_contains_inserted_cities(service_client, cities):
    """Test that inserted cities appear in the response"""
    response = await service_client.get('/city')
    assert response.status == 200
    data = response.json()
    returned_ids = {item['id'] for item in data}
    for city in cities:
        assert city['id'] in returned_ids


async def test_city_list_item_has_required_fields(service_client, cities):
    """Test that each city item has id and name fields"""
    response = await service_client.get('/city')
    assert response.status == 200
    data = response.json()
    assert len(data) > 0
    for item in data:
        assert 'id' in item
        assert 'name' in item


async def test_city_list_no_auth_required(service_client):
    """Test that GET /city does not require authentication"""
    response = await service_client.get('/city')
    assert response.status != 401

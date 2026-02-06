import pytest


async def test_health(service_client):
    response = await service_client.get('/health')

    assert response.status == 200
    assert response.text == 'OK'

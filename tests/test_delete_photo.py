import uuid

import pytest

S3_PUBLIC_BASE = 'https://hollow1crown.storage.yandexcloud.net'


def make_unique_email(prefix='delete'):
    return f'{prefix}_{uuid.uuid4().hex[:10]}@example.com'


def make_photo_url(ext='jpg'):
    return f'{S3_PUBLIC_BASE}/photos/{uuid.uuid4().hex}.{ext}'


@pytest.fixture
async def auth_token(service_client):
    email = make_unique_email()
    password = 'TestPassword123'

    await service_client.post(
        '/user/register',
        json={
            'email': email,
            'password': password,
            'first_name': 'Delete',
            'last_name': 'Tester',
        },
    )
    login = await service_client.post(
        '/auth/login',
        json={'email': email, 'password': password},
    )
    return login.json()['access_token']


async def test_delete_photo_requires_auth(service_client):
    response = await service_client.delete(
        '/delete/photo',
        json={'url': make_photo_url()},
    )
    assert response.status == 401


async def test_delete_photo_missing_url_field(service_client, auth_token):
    response = await service_client.delete(
        '/delete/photo',
        json={},
        headers={'Authorization': f'Bearer {auth_token}'},
    )
    assert response.status == 400
    assert response.json()['error']['code'] == 'MISSING_FIELD'


async def test_delete_photo_empty_url(service_client, auth_token):
    response = await service_client.delete(
        '/delete/photo',
        json={'url': ''},
        headers={'Authorization': f'Bearer {auth_token}'},
    )
    assert response.status == 400
    assert response.json()['error']['code'] == 'MISSING_FIELD'


async def test_delete_photo_invalid_url(service_client, auth_token):
    response = await service_client.delete(
        '/delete/photo',
        json={'url': 'https://other-bucket.example.com/photos/abc.jpg'},
        headers={'Authorization': f'Bearer {auth_token}'},
    )
    assert response.status == 400
    assert response.json()['error']['code'] == 'VALIDATION_ERROR'


async def test_delete_photo_success(service_client, auth_token, mockserver):
    @mockserver.handler('/photos/', prefix=True)
    def s3_delete(request):
        assert request.method == 'DELETE'
        return mockserver.make_response('', 204)

    response = await service_client.delete(
        '/delete/photo',
        json={'url': make_photo_url('jpg')},
        headers={'Authorization': f'Bearer {auth_token}'},
    )
    assert response.status == 204


async def test_delete_photo_png_success(service_client, auth_token, mockserver):
    @mockserver.handler('/photos/', prefix=True)
    def s3_delete(request):
        assert request.method == 'DELETE'
        return mockserver.make_response('', 204)

    response = await service_client.delete(
        '/delete/photo',
        json={'url': make_photo_url('png')},
        headers={'Authorization': f'Bearer {auth_token}'},
    )
    assert response.status == 204

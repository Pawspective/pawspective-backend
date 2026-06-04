import uuid

import pytest


def make_unique_email(prefix='upload'):
    return f'{prefix}_{uuid.uuid4().hex[:10]}@example.com'


TINY_JPEG = (
    b'\xff\xd8\xff\xe0\x00\x10JFIF\x00\x01\x01\x00\x00\x01\x00\x01\x00\x00'
    b'\xff\xdb\x00C\x00\x08\x06\x06\x07\x06\x05\x08\x07\x07\x07\t\t'
    b'\x08\n\x0c\x14\r\x0c\x0b\x0b\x0c\x19\x12\x13\x0f\x14\x1d\x1a'
    b'\x1f\x1e\x1d\x1a\x1c\x1c $.\' ",#\x1c\x1c(7),01444\x1f\'9=82<.342\x87'
    b'\xff\xc0\x00\x0b\x08\x00\x01\x00\x01\x01\x01\x11\x00\xff\xc4\x00\x1f'
    b'\x00\x00\x01\x05\x01\x01\x01\x01\x01\x01\x00\x00\x00\x00\x00\x00\x00'
    b'\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\xff\xda\x00\x08\x01'
    b'\x01\x00\x00?\x00\xfb\xd4\xff\xd9'
)

TINY_PNG = (
    b'\x89PNG\r\n\x1a\n'
    b'\x00\x00\x00\rIHDR\x00\x00\x00\x01\x00\x00\x00\x01\x08\x02'
    b'\x00\x00\x00\x90wS\xde'
    b'\x00\x00\x00\x0cIDATx\x9cc\xf8\x0f\x00\x00\x01\x01\x00\x05\x18\xd8N'
    b'\x00\x00\x00\x00IEND\xaeB`\x82'
)


@pytest.fixture
async def auth_token(service_client):
    email = make_unique_email()
    password = 'TestPassword123'

    await service_client.post(
        '/user/register',
        json={
            'email': email,
            'password': password,
            'first_name': 'Upload',
            'last_name': 'Tester',
        },
    )
    login = await service_client.post(
        '/auth/login',
        json={'email': email, 'password': password},
    )
    return login.json()['access_token']


async def test_upload_photo_requires_auth(service_client):
    response = await service_client.post(
        '/upload/photo',
        data=TINY_JPEG,
        headers={'Content-Type': 'image/jpeg'},
    )
    assert response.status == 401


async def test_upload_photo_invalid_content_type(service_client, auth_token):
    response = await service_client.post(
        '/upload/photo',
        data=b'{}',
        headers={
            'Content-Type': 'application/json',
            'Authorization': f'Bearer {auth_token}',
        },
    )
    assert response.status == 400
    assert response.json()['error']['code'] == 'INVALID_CONTENT_TYPE'


async def test_upload_photo_empty_body(service_client, auth_token):
    response = await service_client.post(
        '/upload/photo',
        data=b'',
        headers={
            'Content-Type': 'image/jpeg',
            'Authorization': f'Bearer {auth_token}',
        },
    )
    assert response.status == 400
    assert response.json()['error']['code'] == 'MISSING_FIELD'


async def test_upload_jpeg_returns_url(service_client, auth_token, mockserver):
    @mockserver.handler('/photos/', prefix=True)
    def s3_put(request):
        assert request.method == 'PUT'
        assert request.headers.get('Content-Type') == 'image/jpeg'
        return mockserver.make_response('', 200, headers={'ETag': '"test-etag"'})

    response = await service_client.post(
        '/upload/photo',
        data=TINY_JPEG,
        headers={
            'Content-Type': 'image/jpeg',
            'Authorization': f'Bearer {auth_token}',
        },
    )

    assert response.status == 200
    data = response.json()
    assert 'url' in data
    assert data['url'].startswith('https://')
    assert 'photos/' in data['url']
    assert data['url'].endswith('.jpg')


async def test_upload_png_returns_url(service_client, auth_token, mockserver):
    @mockserver.handler('/photos/', prefix=True)
    def s3_put(request):
        assert request.method == 'PUT'
        return mockserver.make_response('', 200, headers={'ETag': '"test-etag"'})

    response = await service_client.post(
        '/upload/photo',
        data=TINY_PNG,
        headers={
            'Content-Type': 'image/png',
            'Authorization': f'Bearer {auth_token}',
        },
    )

    assert response.status == 200
    assert response.json()['url'].endswith('.png')

from urllib.parse import urlparse

import pytest
from pytest_userver import plugins
from testsuite.databases.pgsql import discover

pytest_plugins = [
    'pytest_userver.plugins.core',
    'pytest_userver.plugins.postgresql',
]

USERVER_CONFIG_HOOKS = ['userver_config_s3']


@pytest.fixture(scope='session')
def userver_default_log_level() -> str:
    """Lower service log verbosity for testsuite runner."""
    return 'debug'


@pytest.fixture(scope='session')
def pgsql_local(service_source_dir, pgsql_local_create):
    databases = discover.find_schemas(
        service_name=None,
        schema_dirs=[service_source_dir / 'postgresql/schemas']
    )
    return pgsql_local_create(list(databases.values()))


@pytest.fixture(scope='session')
def userver_pg_config(pgsql_local):
    def _patch_config(config_yaml, config_vars):
        db_info = pgsql_local['postgres-db']

        components = config_yaml['components_manager']['components']

        if 'postgres-db' not in components:
            components['postgres-db'] = {}

        components['postgres-db']['dbconnection'] = db_info.get_uri()

    return _patch_config


@pytest.fixture(scope='session')
def userver_config_s3(mockserver_info):
    def _patch_config(config_yaml, config_vars):
        parsed = urlparse(mockserver_info.base_url)
        s3 = config_yaml['components_manager']['components']['s3-service']
        # Route S3 requests to mockserver; localhost: triggers path-style URLs in userver s3api
        s3['endpoint'] = f'http://{parsed.hostname}:{parsed.port}'

    return _patch_config

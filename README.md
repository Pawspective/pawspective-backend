# Pawspective

## Project Structure

The project follows a structured folder organization where each layer has a clear
and dedicated responsibility.

### Directories

- **handlers/** – Handle requests and responses (controllers/routes)
- **services/** – Business logic and application rules
- **repositories/** – Data access and persistence (DB/external services)
- **models/** – Domain models and entities
- **dto/** – Request/response schemas and data transfer objects
- **utils/** – Shared helpers and common utilities

## Makefile

Makefile contains typicaly useful targets for development:

- `make build-debug` - debug build of the service with all the assertions and
sanitizers enabled
- `make build-release` - release build of the service with LTO
- `make test-debug` - does a `make build-debug` and runs all the tests on the result
- `make test-release` - does a `make build-release` and runs all the tests on
the result
- `make start-debug` - builds the service in debug mode and starts it
- `make start-release` - builds the service in release mode and starts it
- `make` or `make all` - builds and runs all the tests in release and debug modes
- `make format` - autoformat all the C++ and Python sources
- `make clean-` - cleans the object files
- `make dist-clean` - clean all, including the CMake cached configurations
- `make install` - does a `make build-release` and runs install in directory
set in environment `PREFIX`
- `make install-debug` - does a `make build-debug` and runs install in directory
set in environment `PREFIX`

Edit `Makefile.local` to change the default configuration and build options.

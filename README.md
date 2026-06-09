# 🐾 Pawspective Backend

**REST API for a pet-care platform, built with C++20 and the userver async framework.**

[![C++](https://img.shields.io/badge/C%2B%2B-20-blue?logo=cplusplus&logoColor=white)](https://en.cppreference.com/w/cpp/20)
[![userver](https://img.shields.io/badge/userver-latest-orange?logo=data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHhtbG5zOnhsaW5rPSJodHRwOi8vd3d3LnczLm9yZy8xOTk5L3hsaW5rIiB2aWV3Qm94PSIyNSAwIDE5OCAxOTgiPjxkZWZzPjxzdHlsZT4uY2xzLTJ7ZmlsbDojZjE4MTI4O30uY2xzLTN7ZmlsbDojMWQxYzI5O30uY2xzLTR7ZmlsbDojZmZmO30uY2xzLTV7Y2xpcC1wYXRoOnVybCgjY2xpcC1wYXRoKTt9PC9zdHlsZT48Y2xpcFBhdGggaWQ9ImNsaXAtcGF0aCIgdHJhbnNmb3JtPSJ0cmFuc2xhdGUoMjUuMTYpIj48cGF0aCBkPSJNMTk4LDk5LjMyQTk5LjA4LDk5LjA4LDAsMCwxLDcxLjM4LDE5NC4xYTk2LjU5LDk2LjU5LDAsMCwxLTE4LjI2LTcuMzZBOTksOTksMCwwLDEsMTAzLjY0LjExQzE1Ni4zNiwyLjUxLDE5OC4xOSw0Ni41NSwxOTgsOTkuMzJaIi8+PC9jbGlwUGF0aD48L2RlZnM+PHBhdGggY2xhc3M9ImNscy0yIiBkPSJNMTk4LDk5LjMyQTk5LjA4LDk5LjA4LDAsMCwxLDcxLjM4LDE5NC4xYTk2LjU5LDk2LjU5LDAsMCwxLTE4LjI2LTcuMzZBOTksOTksMCwwLDEsMTAzLjY0LjExQzE1Ni4zNiwyLjUxLDE5OC4xOSw0Ni41NSwxOTgsOTkuMzJaIiB0cmFuc2Zvcm09InRyYW5zbGF0ZSgyNS4xNikiLz48cGF0aCBjbGFzcz0iY2xzLTMiIGQ9Ik0xMjIuMzgsNDEuMjloMGE0MC4wNyw0MC4wNywwLDAsMSw0MC4wNyw0MC4wN3Y0OS4yNWEwLDAsMCwwLDEsMCwwSDgyLjMxYTAsMCwwLDAsMSwwLDBWODEuMzZhNDAuMDcsNDAuMDcsMCwwLDEsNDAuMDctNDAuMDdaIi8+PGNpcmNsZSBjbGFzcz0iY2xzLTQiIGN4PSIxMDQuMTkiIGN5PSI4NS45NiIgcj0iNC45NiIvPjxjaXJjbGUgY2xhc3M9ImNscy00IiBjeD0iMTQwLjU4IiBjeT0iODUuOTYiIHI9IjQuOTYiLz48ZyBjbGFzcz0iY2xzLTUiPjxwYXRoIGNsYXNzPSJjbHMtMyIgZD0iTTUyLjUsMTU4LjM2YTIxLjIyLDIxLjIyLDAsMCwxLTIxLjE5LTIxLjE5VjEwOC45M0ExNy4zNiwxNy4zNiwwLDAsMCwxNCw5MS41OUgtMjUuMTZWODcuNzRIMTRhMjEuMjIsMjEuMjIsMCwwLDEsMjEuMiwyMS4xOXYyOC4yNGExNy4zNCwxNy4zNCwwLDAsMCwzNC42NywwVjExOC4yM2gzLjg1djE4Ljk0QTIxLjIxLDIxLjIxLDAsMCwxLDUyLjUsMTU4LjM2WiIgdHJhbnNmb3JtPSJ0cmFuc2xhdGUoMjUuMTYpIi8+PHJlY3QgY2xhc3M9ImNscy0zIiB4PSIxMTIuMjIiIHk9IjExOS45NCIgd2lkdGg9IjQuMTEiIGhlaWdodD0iODIuODIiLz48cmVjdCBjbGFzcz0iY2xzLTMiIHg9IjEyOS41NyIgeT0iMTE5Ljk0IiB3aWR0aD0iNC4xMSIgaGVpZ2h0PSI4Mi44MiIvPjxwYXRoIGNsYXNzPSJjbHMtMyIgZD0iTTE3MS42MywyMjUuMDhIMTIxLjc2VjEyNi43Nmg0LjExVjIyMWg0MS42NVYxNDAuODhhMTYuMTgsMTYuMTgsMCwwLDEsMTYuMTYtMTYuMTdoMjV2NC4xMWgtMjVhMTIuMDYsMTIuMDYsMCwwLDAtMTIuMDUsMTIuMDZaIiB0cmFuc2Zvcm09InRyYW5zbGF0ZSgyNS4xNikiLz48L2c+PC9zdmc+Cg==)](https://userver.tech)
[![PostgreSQL](https://img.shields.io/badge/PostgreSQL-16-4169E1?logo=postgresql&logoColor=white)](https://www.postgresql.org/)
[![Docker](https://img.shields.io/badge/Docker-ready-2496ED?logo=docker&logoColor=white)](https://www.docker.com/)
[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE.md)
[![CI](https://github.com/Pawspective/pawspective-backend/actions/workflows/ci.yml/badge.svg)](https://github.com/Pawspective/pawspective-backend/actions)

---

[Quick Start](#-quick-start) · [Architecture](#-architecture) · [API](#-api-overview) · [Configuration](#-configuration) · [Contributing](#-contributing)

</div>

---

## 📖 About

Pawspective is a pet-care platform backend that provides a robust and scalable API for managing pets, shelters, breeds, vets, and more. The service is written in **C++20** using the [userver](https://userver.tech) asynchronous framework.

---

## 🚀 Quick Start

### Option A — Docker Compose (recommended)

The fastest way to get a fully working stack (service + PostgreSQL + nginx) running:

**1. Download the latest release bundle**

Go to the [Releases page](https://github.com/Pawspective/pawspective-backend/releases) and download the latest `pawspective-backend.tar.gz`, then extract it:

```bash
tar -xzf pawspective-backend.tar.gz
cd pawspective-backend
```

**2. Prepare secrets**

```bash
mkdir -p secrets
cp secdist.example.json secrets/secdist.json
# Edit secrets/secdist.json and fill in real credentials
```

**3. Set required environment variables**

Create a `.env` file in the project root:

```env
POSTGRES_PASSWORD=your_strong_password
```

**4. Configure nginx and TLS**

Edit `nginx/nginx.conf` and set your domain name. Then obtain a TLS certificate with certbot or place your existing certificates under `certbot/conf/`.

**5. Start the stack**

```bash
docker compose up -d
```

The API will be available via nginx on ports `80`/`443`. The service itself listens internally on port `8080`.

---

### Option B — Build from source (with Dev Containers)

1. Open the repository in VS Code and agree to *"Reopen in Container"* when prompted.
   Docker will pull the pre-built `ubuntu-24.04-userver` image (~6 GB, one-time download).

2. Configure and build:

```bash
make build-debug     # Debug build with AddressSanitizer + UBSan
make build-release   # Optimized release build
```

> `secdist.json` is created automatically from `secdist.example.json` on first run.

3. Run tests:

```bash
make test-debug
```

4. Start the service locally:

```bash
make start-debug
# Service is listening on http://127.0.0.1:8080
```

5. Smoke-test with health check:

```bash
curl http://localhost:8080/health
# HTTP 200 OK
```
---

## 🏗 Architecture

```
pawspective-backend/
├── src/
│   ├── handlers/       # HTTP request handlers (controllers / routes)
│   ├── services/       # Business logic and application rules
│   ├── repositories/   # Data access layer (PostgreSQL queries)
│   ├── models/         # Domain entities and value objects
│   ├── dto/            # Request/response schemas (DTOs)
│   └── utils/          # Shared helpers and utilities
│
├── configs/
│   ├── static_config.yaml   # Component wiring, ports, task processors
│   └── config_vars.yaml     # Environment-specific variable overrides
│
├── postgresql/
│   └── schemas/        # SQL schema migrations (applied on DB start)
│
├── nginx/              # nginx configuration
├── certbot/            # TLS certificate management (Let's Encrypt)
├── secrets/            # Runtime secrets (not committed to VCS)
│
├── tests/              # Functional tests powered by userver testsuite (pytest)
│
├── Dockerfile          # Multi-stage build: builder → minimal runtime image
├── docker-compose.yml  # Full stack: service + PostgreSQL + nginx + certbot
└── secdist.example.json # Template for secrets (DB creds, JWT key)
```

---

## 🔌 API Overview

All endpoints are served on **port 8080** (proxied through nginx). The base path follows RESTful conventions.

| Method | Path | Auth | Description |
|--------|------|------|-------------|
| `GET` | `/health` | — | Health check — returns `200 OK` when the service is ready |
| `POST` | `/user/register` | — | Register a new user account |
| `POST` | `/auth/login` | — | Authenticate and receive a JWT token |
| `POST` | `/auth/refresh` | — | Refresh JWT tokens |
| `POST` | `/auth/logout` | ✅ | Log out and invalidate session |
| `GET` | `/auth/me` | ✅ | Get current authenticated user |
| `PUT` | `/user/{id}` | ✅ | Update user profile |
| `DELETE` | `/user/{id}` | ✅ | Delete user account |
| `GET` | `/city` | — | List available cities |
| `GET` | `/breeds` | — | List all available breeds |
| `GET` | `/orgs` | — | Search organizations/shelters |
| `POST` | `/orgs` | ✅ | Register a new organization |
| `GET` | `/orgs/{id}` | — | Get organization by ID |
| `PUT` | `/orgs/{id}` | ✅ | Update organization |
| `DELETE` | `/orgs/{id}` | ✅ | Delete organization |
| `GET` | `/orgs/{id}/animals` | — | List animals in an organization |
| `GET` | `/animals` | — | List animals (filterable) |
| `GET` | `/animals/filters` | — | Get available filter options |
| `POST` | `/animals` | ✅ | Create a new animal listing |
| `GET` | `/animals/{id}` | — | Get animal by ID |
| `PUT` | `/animals/{id}` | ✅ | Update animal listing |
| `DELETE` | `/animals/{id}` | ✅ | Remove animal listing |
| `POST` | `/animals/{id}/adopt` | ✅ | Submit an adoption request |
| `GET` | `/requests` | ✅ | List adoption requests |
| `POST` | `/requests/accept/{id}` | ✅ | Accept an adoption request |
| `POST` | `/requests/deny/{id}` | ✅ | Deny an adoption request |
| `GET` | `/me/adopted-animals/pending-review` | ✅ | List adopted animals awaiting review |
| `GET` | `/posts` | — | List posts |
| `POST` | `/posts` | ✅ | Create a post |
| `PUT` | `/posts/{id}` | ✅ | Update a post |
| `DELETE` | `/posts/{id}` | ✅ | Delete a post |
| `GET` | `/reviews` | — | List reviews |
| `POST` | `/reviews` | ✅ | Create a review |
| `PUT` | `/reviews/{id}` | ✅ | Update a review |
| `DELETE` | `/reviews/{id}` | ✅ | Delete a review |
| `POST` | `/upload/photo` | ✅ | Upload a photo |

> ✅ — requires `Authorization: Bearer <token>` header.

---

## ⚙️ Configuration

### Secrets — `secrets/secdist.json`

Sensitive credentials are kept **outside** the static config in `secrets/secdist.json` (never committed to VCS). Create it from the template:

```bash
mkdir -p secrets
cp secdist.example.json secrets/secdist.json
```

```json
{
  "jwt-service": {
    "secret_key": "your-strong-secret-key"
  },
  "s3-service": {
    "access_key": "CLOUD_ACCESS_KEY_ID",
    "secret_key": "CLOUD_SECRET_ACCESS_KEY",
    "bucket": "your-bucket-name",
    "endpoint": "storage.yandexcloud.net",
    "public_url_base": "https://your-bucket-name.storage.yandexcloud.net"
  },
  "postgresql_settings": {
    "databases": {
      "postgres-db": [
        {
          "shard_number": 0,
          "hosts": [
            "host=postgres port=5432 dbname=pawspective user=pawspective_user password=your_password"
          ]
        }
      ]
    }
  }
}
```

### Static config — `configs/static_config.yaml`

Controls component wiring, task processors, port numbers, and observability settings. References variables from `configs/config_vars.yaml` via `$variable-name` syntax.

### Environment variables (Docker Compose)

| Variable | Required | Description |
|---|---|---|
| `POSTGRES_PASSWORD` | ✅ | PostgreSQL password (set in `.env` file) |

The PostgreSQL username and database name are hardcoded in `docker-compose.yml` as `pawspective_user` / `pawspective`.

---

## 🛠 Development

### Makefile targets

| Target | Description |
|---|---|
| `make build-debug` | Debug build with AddressSanitizer + UBSan |
| `make build-release` | Release build with LTO |
| `make build-debug-custom` | Debug build with custom CMake preset |
| `make build-release-custom` | Release build with custom CMake preset |
| `make test-debug` | Build debug + run all tests |
| `make test-release` | Build release + run all tests |
| `make start-debug` | Build and start the service in debug mode |
| `make start-release` | Build and start the service in release mode |
| `make format` | Auto-format C++ sources (clang-format) and Python tests (autopep8) |
| `make lint` | Run format-check, clang-tidy, and cppcheck |
| `make tidy` | Run clang-tidy only |
| `make cppcheck` | Run cppcheck only |
| `make install` | Release build + install to `$PREFIX` (default `~/.local`) |
| `make clean-debug` | Clean debug build artefacts |
| `make dist-clean` | Remove all build directories and caches |

Override defaults without modifying the Makefile by creating `Makefile.local`:

```makefile
# Makefile.local (not committed)
NPROCS = 8
CMAKE_OPTS = -DCMAKE_VERBOSE_MAKEFILE=ON
```

### Code style

- **C++20**, formatted with **clang-format** (see `.clang-format`).
- Checked with **clang-tidy** (`.clang-tidy`) and **cppcheck** (`.cppcheck_suppressions`).
- Python tests formatted with **autopep8**.
- The CI pipeline enforces all linting rules on every pull request.

---

## 🧪 Testing

Functional tests are written in **pytest** and run via the userver testsuite infrastructure, which automatically starts the service, applies the database schema, and provides request mocking.

```bash
# Run all functional tests
make test-debug

# Run a specific test file
cd build-debug && pytest ../tests/test_auth.py -v
```

The CI workflow (`.github/workflows/`) runs both debug and release test suites on every push and pull request.

---

## 🐳 Docker

The `Dockerfile` uses a **multi-stage build**:

1. **`builder`** stage — `ghcr.io/userver-framework/ubuntu-24.04-userver:latest` with all build tools pre-installed. Compiles the release binary.
2. **`runtime`** stage — minimal `ubuntu:22.04` with only runtime libraries. Produces a small final image.

```bash
# Build the image manually
docker build -t pawspective-backend:latest .

# Run with existing secrets and configs
docker run -p 8080:8080 \
  -v "$(pwd)/secrets/secdist.json:/app/secrets/secdist.json:ro" \
  -v "$(pwd)/configs:/app/etc/pawspective:ro" \
  pawspective-backend:latest
```

The `docker-compose.yml` wires the service together with PostgreSQL 16, nginx, and certbot. PostgreSQL schema migrations are applied automatically on first start via `postgresql/schemas/`.

---

## 🤝 Contributing

We welcome contributions! Please read [CONTRIBUTING.md](CONTRIBUTING.md) for the full guidelines. The short version:

- Every change starts with a **GitHub Issue**.
- Branch naming: `feature/<issue-id>-short-description` or `fix/<issue-id>-short-description`.
- Each PR references exactly one issue.
- Commits follow **Conventional Commits**: `<type>(<scope>): <description>`.

**Commit types:** `feat`, `fix`, `docs`, `style`, `refactor`, `perf`, `test`, `build`, `ci`, `chore`, `revert`.

**Example:**

```
feat(auth): add JWT refresh token endpoint
fix(animals): correct pagination offset calculation
```

Direct pushes to `main` are disabled — all changes go through reviewed pull requests.

---

## 📄 License

Distributed under the **MIT License**. See [LICENSE.md](LICENSE.md) for details.


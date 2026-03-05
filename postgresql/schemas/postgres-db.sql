CREATE TABLE IF NOT EXISTS service_table (
    key TEXT PRIMARY KEY,
    value TEXT
);

-- Begin Extensions
CREATE EXTENSION IF NOT EXISTS pg_trgm;
-- End Extensions

CREATE TABLE IF NOT EXISTS cities (
    id BIGSERIAL PRIMARY KEY,
    name VARCHAR(255) NOT NULL UNIQUE
);

CREATE TABLE IF NOT EXISTS organizations (
    id BIGSERIAL PRIMARY KEY,
    name VARCHAR(255) NOT NULL,
    description TEXT,
    city_id BIGINT NOT NULL,
    CONSTRAINT fk_organizations_city FOREIGN KEY (city_id) REFERENCES cities(id) ON DELETE RESTRICT
);

CREATE TABLE IF NOT EXISTS users (
    id BIGSERIAL PRIMARY KEY,
    email VARCHAR(255) NOT NULL UNIQUE,
    first_name VARCHAR(255) NOT NULL,
    last_name VARCHAR(255) NOT NULL,
    organization_id BIGINT,
    password_hash TEXT NOT NULL,
    CONSTRAINT fk_users_organization FOREIGN KEY (organization_id) REFERENCES organizations(id) ON DELETE SET NULL
);

CREATE INDEX IF NOT EXISTS idx_users_email ON users (email);
CREATE INDEX IF NOT EXISTS idx_users_organization_id ON users (organization_id);
CREATE INDEX IF NOT EXISTS idx_organizations_city_id ON organizations (city_id);
CREATE INDEX IF NOT EXISTS idx_org_name_trgm ON organizations USING gin (name gin_trgm_ops);

CREATE SCHEMA IF NOT EXISTS auth_schema;

CREATE TABLE IF NOT EXISTS auth_schema.sessions (
    id BIGSERIAL PRIMARY KEY,
    user_id BIGINT NOT NULL,
    refresh_token_hash VARCHAR(64) NOT NULL,
    expires_at TIMESTAMPTZ NOT NULL,
    created_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),
    CONSTRAINT fk_sessions_user FOREIGN KEY (user_id) REFERENCES public.users(id) ON DELETE CASCADE
);

CREATE UNIQUE INDEX IF NOT EXISTS idx_sessions_refresh_hash
    ON auth_schema.sessions (refresh_token_hash);

CREATE INDEX IF NOT EXISTS idx_sessions_user_id
    ON auth_schema.sessions (user_id);

CREATE TYPE animal_size AS ENUM
    ('small', 'medium', 'large');

CREATE TYPE animal_gender AS ENUM
    ('male', 'female', 'unknown');

CREATE TYPE care_level AS ENUM 
    ('easy', 'moderate', 'difficult', 'special_needs');

CREATE TYPE good_with AS ENUM 
    ('dogs', 'cats', 'children', 'elderly');

CREATE TYPE animal_color AS ENUM 
    ('black', 'white', 'brown', 'grey', 'orange', 'cream', 'tan', 'golden', 'spotted', 'striped', 'brindle', 'mixed');

CREATE TYPE animal_status AS ENUM 
    ('available', 'adopted', 'unavailable');

CREATE TYPE animal_type AS ENUM 
    ('dog', 'cat', 'other');

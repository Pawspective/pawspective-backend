CREATE TABLE IF NOT EXISTS service_table (
    key TEXT PRIMARY KEY,
    value TEXT
);

-- Begin Extensions
CREATE EXTENSION IF NOT EXISTS pg_trgm;
-- End Extensions

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

CREATE TABLE IF NOT EXISTS breeds (
    id BIGSERIAL PRIMARY KEY,
    animal_type animal_type NOT NULL,
    name VARCHAR(255) NOT NULL,
    CONSTRAINT unique_breed_per_type UNIQUE(animal_type, name)
);

CREATE TABLE IF NOT EXISTS animals (
    id BIGSERIAL PRIMARY KEY,
    organization_id BIGINT NOT NULL,
    name VARCHAR(255) NOT NULL,
    breed_id BIGINT NOT NULL,
    size animal_size NOT NULL,       
    gender animal_gender NOT NULL,    
    care_level care_level NOT NULL,   
    good_with good_with NOT NULL,     
    color animal_color NOT NULL,      
    age INT NOT NULL,
    description TEXT,
    status animal_status NOT NULL DEFAULT 'available',
    
    CONSTRAINT fk_animals_organization FOREIGN KEY (organization_id) REFERENCES organizations(id) ON DELETE CASCADE,
    CONSTRAINT fk_animals_breed FOREIGN KEY (breed_id) REFERENCES breeds(id) ON DELETE RESTRICT
);

CREATE INDEX IF NOT EXISTS idx_users_email ON users (email);
CREATE INDEX IF NOT EXISTS idx_users_organization_id ON users (organization_id);
CREATE INDEX IF NOT EXISTS idx_organizations_city_id ON organizations (city_id);
CREATE INDEX IF NOT EXISTS idx_org_name_trgm ON organizations USING gin (name gin_trgm_ops);
CREATE INDEX IF NOT EXISTS idx_animals_org_id ON animals(organization_id);
CREATE INDEX IF NOT EXISTS idx_animals_breed_id ON animals(breed_id);
CREATE INDEX IF NOT EXISTS idx_animals_status ON animals(status);

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



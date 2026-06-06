CREATE TABLE IF NOT EXISTS service_table (
    key TEXT PRIMARY KEY,
    value TEXT
);

CREATE TABLE IF NOT EXISTS service_distlocks (
    key             TEXT PRIMARY KEY,
    owner           TEXT,
    expiration_time TIMESTAMPTZ
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
    avatar_url TEXT,
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
    photos TEXT[] NOT NULL DEFAULT '{}',
    breed_id BIGINT NOT NULL,
    size animal_size NOT NULL,       
    gender animal_gender NOT NULL,    
    care_level care_level NOT NULL,   
    good_with good_with NOT NULL,     
    color animal_color NOT NULL,      
    age INT NOT NULL,
    user_id BIGINT,
    description TEXT,
    status animal_status NOT NULL DEFAULT 'available',
    
    CONSTRAINT fk_animals_organization FOREIGN KEY (organization_id) REFERENCES organizations(id) ON DELETE CASCADE,
    CONSTRAINT fk_animals_breed FOREIGN KEY (breed_id) REFERENCES breeds(id) ON DELETE RESTRICT,
    CONSTRAINT fk_animals_user FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE SET NULL
);

CREATE TABLE IF NOT EXISTS posts (
    id BIGSERIAL PRIMARY KEY,
    organization_id BIGINT NOT NULL,
    text TEXT NOT NULL,
    photos TEXT[] NOT NULL DEFAULT '{}',
    created_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),
    CONSTRAINT fk_posts_organization FOREIGN KEY (organization_id) REFERENCES organizations(id) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS reviews (
    id BIGSERIAL PRIMARY KEY,
    animal_id BIGINT NOT NULL,
    user_id BIGINT NOT NULL,
    text TEXT NOT NULL,
    created_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),
    
    CONSTRAINT fk_reviews_animal FOREIGN KEY (animal_id) REFERENCES animals(id) ON DELETE CASCADE,
    CONSTRAINT fk_reviews_user FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE,
    CONSTRAINT unique_review_per_user_animal UNIQUE(user_id, animal_id)
);

CREATE INDEX IF NOT EXISTS idx_users_email ON users (email);
CREATE INDEX IF NOT EXISTS idx_users_organization_id ON users (organization_id);
CREATE INDEX IF NOT EXISTS idx_organizations_city_id ON organizations (city_id);
CREATE INDEX IF NOT EXISTS idx_org_name_trgm ON organizations USING gin (name gin_trgm_ops);
CREATE INDEX IF NOT EXISTS idx_animals_org_id ON animals(organization_id);
CREATE INDEX IF NOT EXISTS idx_animals_breed_id ON animals(breed_id);
CREATE INDEX IF NOT EXISTS idx_animals_status ON animals(status);
CREATE INDEX IF NOT EXISTS idx_posts_organization_id ON posts(organization_id);
CREATE INDEX IF NOT EXISTS idx_posts_created_at ON posts(created_at DESC);
CREATE INDEX IF NOT EXISTS idx_reviews_animal_id ON reviews(animal_id);
CREATE INDEX IF NOT EXISTS idx_reviews_created_at ON reviews(created_at DESC);

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

CREATE TABLE IF NOT EXISTS adopt_requests (
    id BIGSERIAL PRIMARY KEY,
    animal_id BIGINT NOT NULL,
    user_id BIGINT NOT NULL,
    CONSTRAINT fk_adopt_requests_animal FOREIGN KEY (animal_id) REFERENCES animals(id) ON DELETE CASCADE,
    CONSTRAINT fk_adopt_requests_user FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE,
    CONSTRAINT unique_adopt_request_per_user_animal UNIQUE (animal_id, user_id)
);

CREATE INDEX IF NOT EXISTS idx_adopt_requests_animal_id ON adopt_requests (animal_id);
CREATE INDEX IF NOT EXISTS idx_adopt_requests_user_id ON adopt_requests (user_id);

CREATE OR REPLACE FUNCTION delete_user_org_callback()
RETURNS TRIGGER AS $$
BEGIN
    IF OLD.organization_id IS NOT NULL THEN
        DELETE FROM organizations WHERE id = OLD.organization_id;
    END IF;
    RETURN OLD;
END;
$$ LANGUAGE plpgsql;

DROP TRIGGER IF EXISTS trg_after_delete_user ON users;
CREATE TRIGGER trg_after_delete_user
AFTER DELETE ON users
FOR EACH ROW 
EXECUTE FUNCTION delete_user_org_callback();

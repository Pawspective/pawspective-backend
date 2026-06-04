BEGIN;

INSERT INTO cities (name)
VALUES
    ('Saint-Petersburg'),
    ('Moscow'),
    ('Kazan')
ON CONFLICT (name) DO NOTHING;

INSERT INTO breeds (animal_type, name)
VALUES
    ('dog', 'Labrador Retriever'),
    ('dog', 'German Shepherd'),
    ('dog', 'Beagle'),
    ('cat', 'Siamese'),
    ('cat', 'Maine Coon'),
    ('cat', 'British Shorthair'),
    ('other', 'Cockatiel'),
    ('other', 'Netherland Dwarf'),
    ('other', 'Syrian Hamster')
ON CONFLICT (animal_type, name) DO NOTHING;

COMMIT;

-- Switch to Accredited Mode
-- This script transitions the system to accredited (university/college) programs
--
-- WARNING: This will DELETE all existing curriculum and department data!
-- Make sure to backup your database before running this script.
--
-- Usage:
--   psql -U your_user -d your_database -f switch_to_accredited.sql
--
-- Or via the API server if configured for migrations.

-- =====================================================
-- STEP 1: Backup check notice
-- =====================================================
DO $$
BEGIN
    RAISE NOTICE '======================================';
    RAISE NOTICE 'SWITCHING TO ACCREDITED MODE';
    RAISE NOTICE '======================================';
    RAISE NOTICE 'This will DELETE all curriculum and department data!';
    RAISE NOTICE 'Ensure you have a backup before proceeding.';
    RAISE NOTICE '';
END $$;

-- =====================================================
-- STEP 2: Clear existing curriculum data
-- =====================================================

-- Clear curriculum form requirements (references curriculum and form_type)
DELETE FROM curriculum_form_requirement;

-- Clear curriculum prerequisites (references curriculum)
DELETE FROM curriculum_prerequisite;

-- Nullify student curriculum references (so we can delete curriculum)
UPDATE student SET curriculum_id = NULL WHERE curriculum_id IS NOT NULL;

-- Clear curriculum table
DELETE FROM curriculum;

-- Clear department table
DELETE FROM department;

-- Reset sequences for clean IDs
ALTER SEQUENCE IF EXISTS curriculum_id_seq RESTART WITH 1;
ALTER SEQUENCE IF EXISTS department_id_seq RESTART WITH 1;
ALTER SEQUENCE IF EXISTS curriculum_form_requirement_id_seq RESTART WITH 1;
ALTER SEQUENCE IF EXISTS curriculum_prerequisite_id_seq RESTART WITH 1;

-- =====================================================
-- STEP 3: Insert accredited departments
-- =====================================================

INSERT INTO department (code, name, dean, contact_email)
VALUES
    ('CS', 'Computer Science', 'Dr. Jane Smith', 'cs@university.edu'),
    ('BUS', 'Business Administration', 'Dr. John Brown', 'business@university.edu'),
    ('ENG', 'Engineering', 'Dr. Robert Johnson', 'engineering@university.edu'),
    ('NUR', 'Nursing', 'Dr. Sarah Williams', 'nursing@university.edu'),
    ('A&S', 'Arts and Sciences', 'Dr. Michael Davis', 'artsci@university.edu')
ON CONFLICT (code) DO UPDATE SET
    name = EXCLUDED.name,
    dean = EXCLUDED.dean,
    contact_email = EXCLUDED.contact_email,
    updated_at = CURRENT_TIMESTAMP;

-- =====================================================
-- STEP 4: Insert accredited curriculum
-- =====================================================

-- Computer Science Programs
INSERT INTO curriculum (code, name, description, department_id, degree_type, credit_hours, duration_semesters, duration_interval, is_active, is_online, is_accredited, program_type)
VALUES
    ('cs_bs', 'Bachelor of Science in Computer Science',
     'Comprehensive program covering software development, algorithms, and systems.',
     (SELECT id FROM department WHERE code = 'CS'), 'bachelor', 120, 8, 'semester', TRUE, FALSE, TRUE, 'academic'),

    ('cs_ms', 'Master of Science in Computer Science',
     'Advanced study in computer science with research opportunities.',
     (SELECT id FROM department WHERE code = 'CS'), 'master', 36, 4, 'semester', TRUE, TRUE, TRUE, 'academic'),

    ('cert_da', 'Certificate in Data Analytics',
     'Professional certificate in data analysis and visualization.',
     (SELECT id FROM department WHERE code = 'CS'), 'certificate', 18, 2, 'semester', TRUE, TRUE, TRUE, 'academic')
ON CONFLICT (code) DO UPDATE SET
    name = EXCLUDED.name,
    description = EXCLUDED.description,
    department_id = EXCLUDED.department_id,
    degree_type = EXCLUDED.degree_type,
    duration_interval = EXCLUDED.duration_interval,
    is_accredited = EXCLUDED.is_accredited,
    program_type = EXCLUDED.program_type,
    updated_at = CURRENT_TIMESTAMP;

-- Business Programs
INSERT INTO curriculum (code, name, description, department_id, degree_type, credit_hours, duration_semesters, duration_interval, is_active, is_online, is_accredited, program_type)
VALUES
    ('bus_bba', 'Bachelor of Business Administration',
     'Broad business education with specialization options.',
     (SELECT id FROM department WHERE code = 'BUS'), 'bachelor', 120, 8, 'semester', TRUE, FALSE, TRUE, 'academic'),

    ('bus_mba', 'Master of Business Administration',
     'Professional graduate degree in business management.',
     (SELECT id FROM department WHERE code = 'BUS'), 'master', 48, 4, 'semester', TRUE, TRUE, TRUE, 'academic'),

    ('cert_pm', 'Certificate in Project Management',
     'Professional certificate in project management methodologies.',
     (SELECT id FROM department WHERE code = 'BUS'), 'certificate', 15, 2, 'semester', TRUE, TRUE, TRUE, 'academic')
ON CONFLICT (code) DO UPDATE SET
    name = EXCLUDED.name,
    description = EXCLUDED.description,
    department_id = EXCLUDED.department_id,
    degree_type = EXCLUDED.degree_type,
    duration_interval = EXCLUDED.duration_interval,
    is_accredited = EXCLUDED.is_accredited,
    program_type = EXCLUDED.program_type,
    updated_at = CURRENT_TIMESTAMP;

-- Engineering Programs
INSERT INTO curriculum (code, name, description, department_id, degree_type, credit_hours, duration_semesters, duration_interval, is_active, is_online, is_accredited, program_type)
VALUES
    ('eng_bsee', 'Bachelor of Science in Electrical Engineering',
     'Study of electrical systems, electronics, and communications.',
     (SELECT id FROM department WHERE code = 'ENG'), 'bachelor', 128, 8, 'semester', TRUE, FALSE, TRUE, 'academic'),

    ('eng_bsme', 'Bachelor of Science in Mechanical Engineering',
     'Design and analysis of mechanical systems.',
     (SELECT id FROM department WHERE code = 'ENG'), 'bachelor', 128, 8, 'semester', TRUE, FALSE, TRUE, 'academic')
ON CONFLICT (code) DO UPDATE SET
    name = EXCLUDED.name,
    description = EXCLUDED.description,
    department_id = EXCLUDED.department_id,
    degree_type = EXCLUDED.degree_type,
    duration_interval = EXCLUDED.duration_interval,
    is_accredited = EXCLUDED.is_accredited,
    program_type = EXCLUDED.program_type,
    updated_at = CURRENT_TIMESTAMP;

-- Nursing Programs
INSERT INTO curriculum (code, name, description, department_id, degree_type, credit_hours, duration_semesters, duration_interval, is_active, is_online, is_accredited, program_type)
VALUES
    ('nur_bsn', 'Bachelor of Science in Nursing',
     'Professional nursing education for healthcare careers.',
     (SELECT id FROM department WHERE code = 'NUR'), 'bachelor', 120, 8, 'semester', TRUE, FALSE, TRUE, 'academic')
ON CONFLICT (code) DO UPDATE SET
    name = EXCLUDED.name,
    description = EXCLUDED.description,
    department_id = EXCLUDED.department_id,
    degree_type = EXCLUDED.degree_type,
    duration_interval = EXCLUDED.duration_interval,
    is_accredited = EXCLUDED.is_accredited,
    program_type = EXCLUDED.program_type,
    updated_at = CURRENT_TIMESTAMP;

-- Arts and Sciences Programs
INSERT INTO curriculum (code, name, description, department_id, degree_type, credit_hours, duration_semesters, duration_interval, is_active, is_online, is_accredited, program_type)
VALUES
    ('art_ba', 'Bachelor of Arts in Psychology',
     'Study of human behavior and mental processes.',
     (SELECT id FROM department WHERE code = 'A&S'), 'bachelor', 120, 8, 'semester', TRUE, FALSE, TRUE, 'academic')
ON CONFLICT (code) DO UPDATE SET
    name = EXCLUDED.name,
    description = EXCLUDED.description,
    department_id = EXCLUDED.department_id,
    degree_type = EXCLUDED.degree_type,
    duration_interval = EXCLUDED.duration_interval,
    is_accredited = EXCLUDED.is_accredited,
    program_type = EXCLUDED.program_type,
    updated_at = CURRENT_TIMESTAMP;

-- =====================================================
-- STEP 5: Insert curriculum prerequisites
-- =====================================================

-- CS MS requires CS BS
INSERT INTO curriculum_prerequisite (curriculum_id, prerequisite_description)
SELECT c.id, 'Bachelor of Science in Computer Science (cs_bs) or equivalent'
FROM curriculum c WHERE c.code = 'cs_ms'
ON CONFLICT DO NOTHING;

-- =====================================================
-- STEP 6: Insert curriculum form requirements
-- =====================================================

-- Bachelor programs
INSERT INTO curriculum_form_requirement (curriculum_id, form_type_id)
SELECT c.id, f.id
FROM curriculum c, form_type f
WHERE c.code IN ('cs_bs', 'bus_bba', 'art_ba')
  AND f.code IN ('personal_info', 'emergency_contact', 'academic_history', 'consent')
ON CONFLICT (curriculum_id, form_type_id) DO NOTHING;

-- Engineering bachelor programs (includes medical_info)
INSERT INTO curriculum_form_requirement (curriculum_id, form_type_id)
SELECT c.id, f.id
FROM curriculum c, form_type f
WHERE c.code IN ('eng_bsee', 'eng_bsme')
  AND f.code IN ('personal_info', 'emergency_contact', 'medical_info', 'academic_history', 'consent')
ON CONFLICT (curriculum_id, form_type_id) DO NOTHING;

-- Nursing bachelor (includes medical_info and documents)
INSERT INTO curriculum_form_requirement (curriculum_id, form_type_id)
SELECT c.id, f.id
FROM curriculum c, form_type f
WHERE c.code = 'nur_bsn'
  AND f.code IN ('personal_info', 'emergency_contact', 'medical_info', 'academic_history', 'documents', 'consent')
ON CONFLICT (curriculum_id, form_type_id) DO NOTHING;

-- Master programs
INSERT INTO curriculum_form_requirement (curriculum_id, form_type_id)
SELECT c.id, f.id
FROM curriculum c, form_type f
WHERE c.code IN ('cs_ms', 'bus_mba')
  AND f.code IN ('personal_info', 'emergency_contact', 'academic_history', 'documents', 'consent')
ON CONFLICT (curriculum_id, form_type_id) DO NOTHING;

-- Certificate programs
INSERT INTO curriculum_form_requirement (curriculum_id, form_type_id)
SELECT c.id, f.id
FROM curriculum c, form_type f
WHERE c.code IN ('cert_da', 'cert_pm')
  AND f.code IN ('personal_info', 'emergency_contact', 'consent')
ON CONFLICT (curriculum_id, form_type_id) DO NOTHING;

-- =====================================================
-- STEP 7: Update institution settings
-- =====================================================

UPDATE institution_settings SET setting_value = 'accredited', updated_at = CURRENT_TIMESTAMP
WHERE setting_key = 'institution_type';

UPDATE institution_settings SET setting_value = 'accredited', updated_at = CURRENT_TIMESTAMP
WHERE setting_key = 'program_mode';

UPDATE institution_settings SET setting_value = '["associate", "bachelor", "master", "doctoral", "certificate"]', updated_at = CURRENT_TIMESTAMP
WHERE setting_key = 'degree_types_enabled';

-- =====================================================
-- STEP 8: Summary
-- =====================================================

DO $$
DECLARE
    dept_count INTEGER;
    curr_count INTEGER;
BEGIN
    SELECT COUNT(*) INTO dept_count FROM department;
    SELECT COUNT(*) INTO curr_count FROM curriculum;

    RAISE NOTICE '';
    RAISE NOTICE '======================================';
    RAISE NOTICE 'ACCREDITED MODE ACTIVATED';
    RAISE NOTICE '======================================';
    RAISE NOTICE 'Departments loaded: %', dept_count;
    RAISE NOTICE 'Programs loaded: %', curr_count;
    RAISE NOTICE '';
    RAISE NOTICE 'Departments:';
    RAISE NOTICE '  - Computer Science';
    RAISE NOTICE '  - Business Administration';
    RAISE NOTICE '  - Engineering';
    RAISE NOTICE '  - Nursing';
    RAISE NOTICE '  - Arts and Sciences';
    RAISE NOTICE '';
    RAISE NOTICE 'Degree Types: Associate, Bachelor, Master, Doctoral, Certificate';
    RAISE NOTICE '======================================';
END $$;

-- Migration 005: Seed Accredited Academic Curriculum
-- This migration seeds accredited academic programs
-- Can be run independently or combined with vocational curriculum
-- Use 006_flush_curriculum.sql to clear data before re-seeding

-- =====================================================
-- DEPARTMENTS - Accredited Academic
-- =====================================================

-- Clear existing departments and add fresh data
-- Note: Use 006_flush_curriculum.sql for complete flush

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
-- CURRICULUM - Computer Science Programs
-- =====================================================

INSERT INTO curriculum (code, name, description, department_id, degree_type, credit_hours, duration_semesters, is_active, is_online, is_accredited, program_type)
VALUES
    ('cs_bs', 'Bachelor of Science in Computer Science',
     'Comprehensive program covering software development, algorithms, and systems.',
     (SELECT id FROM department WHERE code = 'CS'), 'bachelor', 120, 8, TRUE, FALSE, TRUE, 'academic'),

    ('cs_ms', 'Master of Science in Computer Science',
     'Advanced study in computer science with research opportunities.',
     (SELECT id FROM department WHERE code = 'CS'), 'master', 36, 4, TRUE, TRUE, TRUE, 'academic'),

    ('cert_da', 'Certificate in Data Analytics',
     'Professional certificate in data analysis and visualization.',
     (SELECT id FROM department WHERE code = 'CS'), 'certificate', 18, 2, TRUE, TRUE, TRUE, 'academic')
ON CONFLICT (code) DO UPDATE SET
    name = EXCLUDED.name,
    description = EXCLUDED.description,
    department_id = EXCLUDED.department_id,
    degree_type = EXCLUDED.degree_type,
    credit_hours = EXCLUDED.credit_hours,
    duration_semesters = EXCLUDED.duration_semesters,
    is_active = EXCLUDED.is_active,
    is_online = EXCLUDED.is_online,
    is_accredited = EXCLUDED.is_accredited,
    program_type = EXCLUDED.program_type,
    updated_at = CURRENT_TIMESTAMP;

-- =====================================================
-- CURRICULUM - Business Programs
-- =====================================================

INSERT INTO curriculum (code, name, description, department_id, degree_type, credit_hours, duration_semesters, is_active, is_online, is_accredited, program_type)
VALUES
    ('bus_bba', 'Bachelor of Business Administration',
     'Broad business education with specialization options.',
     (SELECT id FROM department WHERE code = 'BUS'), 'bachelor', 120, 8, TRUE, FALSE, TRUE, 'academic'),

    ('bus_mba', 'Master of Business Administration',
     'Professional graduate degree in business management.',
     (SELECT id FROM department WHERE code = 'BUS'), 'master', 48, 4, TRUE, TRUE, TRUE, 'academic'),

    ('cert_pm', 'Certificate in Project Management',
     'Professional certificate in project management methodologies.',
     (SELECT id FROM department WHERE code = 'BUS'), 'certificate', 15, 2, TRUE, TRUE, TRUE, 'academic')
ON CONFLICT (code) DO UPDATE SET
    name = EXCLUDED.name,
    description = EXCLUDED.description,
    department_id = EXCLUDED.department_id,
    degree_type = EXCLUDED.degree_type,
    credit_hours = EXCLUDED.credit_hours,
    duration_semesters = EXCLUDED.duration_semesters,
    is_active = EXCLUDED.is_active,
    is_online = EXCLUDED.is_online,
    is_accredited = EXCLUDED.is_accredited,
    program_type = EXCLUDED.program_type,
    updated_at = CURRENT_TIMESTAMP;

-- =====================================================
-- CURRICULUM - Engineering Programs
-- =====================================================

INSERT INTO curriculum (code, name, description, department_id, degree_type, credit_hours, duration_semesters, is_active, is_online, is_accredited, program_type)
VALUES
    ('eng_bsee', 'Bachelor of Science in Electrical Engineering',
     'Study of electrical systems, electronics, and communications.',
     (SELECT id FROM department WHERE code = 'ENG'), 'bachelor', 128, 8, TRUE, FALSE, TRUE, 'academic'),

    ('eng_bsme', 'Bachelor of Science in Mechanical Engineering',
     'Design and analysis of mechanical systems.',
     (SELECT id FROM department WHERE code = 'ENG'), 'bachelor', 128, 8, TRUE, FALSE, TRUE, 'academic')
ON CONFLICT (code) DO UPDATE SET
    name = EXCLUDED.name,
    description = EXCLUDED.description,
    department_id = EXCLUDED.department_id,
    degree_type = EXCLUDED.degree_type,
    credit_hours = EXCLUDED.credit_hours,
    duration_semesters = EXCLUDED.duration_semesters,
    is_active = EXCLUDED.is_active,
    is_online = EXCLUDED.is_online,
    is_accredited = EXCLUDED.is_accredited,
    program_type = EXCLUDED.program_type,
    updated_at = CURRENT_TIMESTAMP;

-- =====================================================
-- CURRICULUM - Nursing Programs
-- =====================================================

INSERT INTO curriculum (code, name, description, department_id, degree_type, credit_hours, duration_semesters, is_active, is_online, is_accredited, program_type)
VALUES
    ('nur_bsn', 'Bachelor of Science in Nursing',
     'Professional nursing education for healthcare careers.',
     (SELECT id FROM department WHERE code = 'NUR'), 'bachelor', 120, 8, TRUE, FALSE, TRUE, 'academic')
ON CONFLICT (code) DO UPDATE SET
    name = EXCLUDED.name,
    description = EXCLUDED.description,
    department_id = EXCLUDED.department_id,
    degree_type = EXCLUDED.degree_type,
    credit_hours = EXCLUDED.credit_hours,
    duration_semesters = EXCLUDED.duration_semesters,
    is_active = EXCLUDED.is_active,
    is_online = EXCLUDED.is_online,
    is_accredited = EXCLUDED.is_accredited,
    program_type = EXCLUDED.program_type,
    updated_at = CURRENT_TIMESTAMP;

-- =====================================================
-- CURRICULUM - Arts and Sciences Programs
-- =====================================================

INSERT INTO curriculum (code, name, description, department_id, degree_type, credit_hours, duration_semesters, is_active, is_online, is_accredited, program_type)
VALUES
    ('art_ba', 'Bachelor of Arts in Psychology',
     'Study of human behavior and mental processes.',
     (SELECT id FROM department WHERE code = 'A&S'), 'bachelor', 120, 8, TRUE, FALSE, TRUE, 'academic')
ON CONFLICT (code) DO UPDATE SET
    name = EXCLUDED.name,
    description = EXCLUDED.description,
    department_id = EXCLUDED.department_id,
    degree_type = EXCLUDED.degree_type,
    credit_hours = EXCLUDED.credit_hours,
    duration_semesters = EXCLUDED.duration_semesters,
    is_active = EXCLUDED.is_active,
    is_online = EXCLUDED.is_online,
    is_accredited = EXCLUDED.is_accredited,
    program_type = EXCLUDED.program_type,
    updated_at = CURRENT_TIMESTAMP;

-- =====================================================
-- CURRICULUM PREREQUISITES - Academic Programs
-- =====================================================

-- CS MS requires CS BS
INSERT INTO curriculum_prerequisite (curriculum_id, prerequisite_description)
SELECT c.id, 'Bachelor of Science in Computer Science (cs_bs) or equivalent'
FROM curriculum c WHERE c.code = 'cs_ms'
ON CONFLICT DO NOTHING;

-- =====================================================
-- CURRICULUM FORM REQUIREMENTS - Academic Programs
-- =====================================================

-- Set form requirements for each curriculum type
-- Bachelor programs: personal_info, emergency_contact, academic_history, consent
-- Master programs: personal_info, emergency_contact, academic_history, documents, consent
-- Certificate programs: personal_info, emergency_contact, consent

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

-- Log the seeding
DO $$
BEGIN
    RAISE NOTICE 'Accredited curriculum seeding completed successfully';
    RAISE NOTICE 'Departments: 5, Curriculums: 10';
END $$;

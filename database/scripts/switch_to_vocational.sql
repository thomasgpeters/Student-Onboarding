-- Switch to Vocational Mode
-- This script transitions the system to vocational (trade school/CDL) programs
--
-- WARNING: This will DELETE all existing curriculum and department data!
-- Make sure to backup your database before running this script.
--
-- Usage:
--   psql -U your_user -d your_database -f switch_to_vocational.sql
--
-- Or via the API server if configured for migrations.

-- =====================================================
-- STEP 1: Backup check notice
-- =====================================================
DO $$
BEGIN
    RAISE NOTICE '======================================';
    RAISE NOTICE 'SWITCHING TO VOCATIONAL MODE';
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
-- STEP 3: Insert vocational departments
-- =====================================================

INSERT INTO department (code, name, dean, contact_email)
VALUES
    ('CDL', 'Professional Driving', 'James Thompson', 'driving@school.edu'),
    ('AUTO', 'Automotive Technology', 'Mike Rodriguez', 'automotive@school.edu'),
    ('FOOD', 'Food Services', 'Sarah Chen', 'culinary@school.edu'),
    ('TRADE', 'Skilled Trades', 'Robert Wilson', 'trades@school.edu'),
    ('ELECT', 'Electrical Technology', 'David Martinez', 'electrical@school.edu'),
    ('WELD', 'Welding Technology', 'John Anderson', 'welding@school.edu'),
    ('HVAC', 'HVAC Technology', 'Lisa Brown', 'hvac@school.edu'),
    ('MED', 'Medical Services', 'Dr. Patricia Lee', 'medical@school.edu')
ON CONFLICT (code) DO UPDATE SET
    name = EXCLUDED.name,
    dean = EXCLUDED.dean,
    contact_email = EXCLUDED.contact_email,
    updated_at = CURRENT_TIMESTAMP;

-- =====================================================
-- STEP 4: Insert CDL curriculum - Class A Programs
-- =====================================================

-- Base Class A CDL Training
INSERT INTO curriculum (
    code, name, description, department_id, degree_type,
    credit_hours, duration_semesters, duration_interval, is_active, is_online,
    is_accredited, program_type, cdl_class, endorsements, is_endorsement,
    training_hours_classroom, training_hours_range, training_hours_road
)
VALUES
    -- Base Class A program (is_endorsement = FALSE)
    ('cdl_class_a', 'Class A CDL Training Program',
     'Non-accredited vocational training for Class A Commercial Driver''s License. Prepares students to operate combination vehicles (tractor-trailers) with a Gross Combination Weight Rating (GCWR) over 26,001 lbs, where the towed vehicle is over 10,000 lbs.',
     (SELECT id FROM department WHERE code = 'CDL'), 'certificate',
     160, 4, 'week', TRUE, FALSE, FALSE, 'vocational', 'A', '[]', FALSE,
     40, 60, 60),

    -- Class A endorsements (is_endorsement = TRUE)
    ('cdl_class_a_doubles_triples', 'Class A CDL - Doubles/Triples Endorsement (T)',
     'Endorsement training for pulling double or triple trailers. The T endorsement allows drivers to pull two or three trailers behind a tractor.',
     (SELECT id FROM department WHERE code = 'CDL'), 'certificate',
     40, 1, 'week', TRUE, FALSE, FALSE, 'vocational', 'A', '["T"]', TRUE,
     8, 16, 16),

    ('cdl_class_a_tanker', 'Class A CDL - Tanker Endorsement (N)',
     'Endorsement training for transporting liquid or gaseous materials in bulk tanks. The N endorsement is required for any tank vehicle with a capacity of 1,000 gallons or more.',
     (SELECT id FROM department WHERE code = 'CDL'), 'certificate',
     40, 1, 'week', TRUE, FALSE, FALSE, 'vocational', 'A', '["N"]', TRUE,
     12, 14, 14),

    ('cdl_class_a_hazmat', 'Class A CDL - Hazardous Materials Endorsement (H)',
     'Endorsement training for transporting hazardous materials. The H endorsement requires TSA background check and allows transport of hazardous materials as defined in 49 CFR 383.5.',
     (SELECT id FROM department WHERE code = 'CDL'), 'certificate',
     32, 1, 'week', TRUE, FALSE, FALSE, 'vocational', 'A', '["H"]', TRUE,
     24, 4, 4),

    ('cdl_class_a_air_brakes', 'Class A CDL - Air Brakes Training',
     'Training to remove the Air Brake restriction from a Class A CDL. Covers air brake system components, dual air brake systems, inspecting air brakes, and using air brakes properly.',
     (SELECT id FROM department WHERE code = 'CDL'), 'certificate',
     24, 3, 'day', TRUE, FALSE, FALSE, 'vocational', 'A', '["air_brakes"]', TRUE,
     8, 8, 8)
ON CONFLICT (code) DO UPDATE SET
    name = EXCLUDED.name,
    description = EXCLUDED.description,
    department_id = EXCLUDED.department_id,
    duration_interval = EXCLUDED.duration_interval,
    is_accredited = EXCLUDED.is_accredited,
    program_type = EXCLUDED.program_type,
    cdl_class = EXCLUDED.cdl_class,
    endorsements = EXCLUDED.endorsements,
    is_endorsement = EXCLUDED.is_endorsement,
    training_hours_classroom = EXCLUDED.training_hours_classroom,
    training_hours_range = EXCLUDED.training_hours_range,
    training_hours_road = EXCLUDED.training_hours_road,
    updated_at = CURRENT_TIMESTAMP;

-- =====================================================
-- STEP 5: Insert CDL curriculum - Class B Programs
-- =====================================================

INSERT INTO curriculum (
    code, name, description, department_id, degree_type,
    credit_hours, duration_semesters, duration_interval, is_active, is_online,
    is_accredited, program_type, cdl_class, endorsements, is_endorsement,
    training_hours_classroom, training_hours_range, training_hours_road
)
VALUES
    -- Base Class B program (is_endorsement = FALSE)
    ('cdl_class_b', 'Class B CDL Training Program',
     'Non-accredited vocational training for Class B Commercial Driver''s License. Prepares students to operate single vehicles with a Gross Vehicle Weight Rating (GVWR) over 26,001 lbs.',
     (SELECT id FROM department WHERE code = 'CDL'), 'certificate',
     120, 3, 'week', TRUE, FALSE, FALSE, 'vocational', 'B', '[]', FALSE,
     30, 45, 45),

    -- Class B endorsements (is_endorsement = TRUE)
    ('cdl_class_b_passenger', 'Class B CDL - Passenger Endorsement (P)',
     'Endorsement training for operating vehicles designed to transport 16 or more passengers including the driver. Covers transit buses, motor coaches, and shuttle buses.',
     (SELECT id FROM department WHERE code = 'CDL'), 'certificate',
     40, 1, 'week', TRUE, FALSE, FALSE, 'vocational', 'B', '["P"]', TRUE,
     12, 14, 14),

    ('cdl_class_b_school_bus', 'Class B CDL - School Bus Endorsement (S)',
     'Endorsement training for operating school buses. Requires P endorsement. Covers student management, school bus laws, loading/unloading at schools and bus stops, and emergency evacuation.',
     (SELECT id FROM department WHERE code = 'CDL'), 'certificate',
     40, 1, 'week', TRUE, FALSE, FALSE, 'vocational', 'B', '["S"]', TRUE,
     16, 12, 12),

    ('cdl_class_b_air_brakes', 'Class B CDL - Air Brakes Training',
     'Training to remove the Air Brake restriction from a Class B CDL. Covers air brake system components, inspecting air brakes, and using air brakes properly.',
     (SELECT id FROM department WHERE code = 'CDL'), 'certificate',
     24, 3, 'day', TRUE, FALSE, FALSE, 'vocational', 'B', '["air_brakes"]', TRUE,
     8, 8, 8)
ON CONFLICT (code) DO UPDATE SET
    name = EXCLUDED.name,
    description = EXCLUDED.description,
    department_id = EXCLUDED.department_id,
    duration_interval = EXCLUDED.duration_interval,
    is_accredited = EXCLUDED.is_accredited,
    program_type = EXCLUDED.program_type,
    cdl_class = EXCLUDED.cdl_class,
    endorsements = EXCLUDED.endorsements,
    is_endorsement = EXCLUDED.is_endorsement,
    training_hours_classroom = EXCLUDED.training_hours_classroom,
    training_hours_range = EXCLUDED.training_hours_range,
    training_hours_road = EXCLUDED.training_hours_road,
    updated_at = CURRENT_TIMESTAMP;

-- =====================================================
-- STEP 6: Insert curriculum prerequisites
-- =====================================================

-- Class A endorsements require Class A base
INSERT INTO curriculum_prerequisite (curriculum_id, prerequisite_description)
SELECT c.id, 'Class A CDL Training Program (cdl_class_a) or equivalent'
FROM curriculum c WHERE c.code IN ('cdl_class_a_doubles_triples', 'cdl_class_a_tanker', 'cdl_class_a_hazmat')
ON CONFLICT DO NOTHING;

-- Class B endorsements require Class B base
INSERT INTO curriculum_prerequisite (curriculum_id, prerequisite_description)
SELECT c.id, 'Class B CDL Training Program (cdl_class_b) or equivalent'
FROM curriculum c WHERE c.code IN ('cdl_class_b_passenger')
ON CONFLICT DO NOTHING;

-- School Bus requires Passenger endorsement
INSERT INTO curriculum_prerequisite (curriculum_id, prerequisite_description)
SELECT c.id, 'Class B CDL Training Program (cdl_class_b) and Passenger Endorsement (cdl_class_b_passenger)'
FROM curriculum c WHERE c.code = 'cdl_class_b_school_bus'
ON CONFLICT DO NOTHING;

-- =====================================================
-- STEP 7: Insert curriculum form requirements
-- =====================================================

-- Base CDL programs (with documents)
INSERT INTO curriculum_form_requirement (curriculum_id, form_type_id)
SELECT c.id, f.id
FROM curriculum c, form_type f
WHERE c.code IN ('cdl_class_a', 'cdl_class_b',
                 'cdl_class_a_doubles_triples', 'cdl_class_a_tanker', 'cdl_class_a_hazmat',
                 'cdl_class_b_passenger', 'cdl_class_b_school_bus')
  AND f.code IN ('personal_info', 'emergency_contact', 'medical_info', 'documents', 'consent')
ON CONFLICT (curriculum_id, form_type_id) DO NOTHING;

-- Air brakes programs (no documents required)
INSERT INTO curriculum_form_requirement (curriculum_id, form_type_id)
SELECT c.id, f.id
FROM curriculum c, form_type f
WHERE c.code IN ('cdl_class_a_air_brakes', 'cdl_class_b_air_brakes')
  AND f.code IN ('personal_info', 'emergency_contact', 'medical_info', 'consent')
ON CONFLICT (curriculum_id, form_type_id) DO NOTHING;

-- =====================================================
-- STEP 8: Update institution settings
-- =====================================================

UPDATE institution_settings SET setting_value = 'vocational', updated_at = CURRENT_TIMESTAMP
WHERE setting_key = 'institution_type';

UPDATE institution_settings SET setting_value = 'vocational', updated_at = CURRENT_TIMESTAMP
WHERE setting_key = 'program_mode';

UPDATE institution_settings SET setting_value = '["certificate"]', updated_at = CURRENT_TIMESTAMP
WHERE setting_key = 'degree_types_enabled';

-- =====================================================
-- STEP 9: Summary
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
    RAISE NOTICE 'VOCATIONAL MODE ACTIVATED';
    RAISE NOTICE '======================================';
    RAISE NOTICE 'Departments loaded: %', dept_count;
    RAISE NOTICE 'Programs loaded: %', curr_count;
    RAISE NOTICE '';
    RAISE NOTICE 'Departments:';
    RAISE NOTICE '  - Professional Driving (CDL)';
    RAISE NOTICE '  - Automotive Technology';
    RAISE NOTICE '  - Food Services';
    RAISE NOTICE '  - Skilled Trades';
    RAISE NOTICE '  - Electrical Technology';
    RAISE NOTICE '  - Welding Technology';
    RAISE NOTICE '  - HVAC Technology';
    RAISE NOTICE '  - Medical Services';
    RAISE NOTICE '';
    RAISE NOTICE 'Degree Types: Certificate only';
    RAISE NOTICE 'CDL Programs: 5 Class A, 4 Class B';
    RAISE NOTICE '======================================';
END $$;

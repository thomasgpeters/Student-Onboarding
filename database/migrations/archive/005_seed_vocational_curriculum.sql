-- Migration 005b: Seed Vocational CDL Curriculum
-- This migration seeds non-accredited vocational programs (CDL training)
-- Can be run independently or combined with accredited curriculum
-- Use 006_flush_curriculum.sql to clear data before re-seeding

-- =====================================================
-- DEPARTMENTS - Vocational/Trade Schools
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
-- CURRICULUM - Class A CDL Programs
-- =====================================================

-- Base Class A CDL Training
INSERT INTO curriculum (
    code, name, description, department_id, degree_type,
    credit_hours, duration_semesters, is_active, is_online,
    is_accredited, program_type, cdl_class, endorsements,
    training_hours_classroom, training_hours_range, training_hours_road
)
VALUES
    ('cdl_class_a', 'Class A CDL Training Program',
     'Non-accredited vocational training for Class A Commercial Driver''s License. Prepares students to operate combination vehicles (tractor-trailers) with a Gross Combination Weight Rating (GCWR) over 26,001 lbs, where the towed vehicle is over 10,000 lbs. Includes classroom instruction, range training, vehicle inspection, and road practice. This is the base Class A program required before adding endorsements.',
     (SELECT id FROM department WHERE code = 'CDL'), 'certificate',
     160, 1, TRUE, FALSE, FALSE, 'vocational', 'A', '[]',
     40, 60, 60)
ON CONFLICT (code) DO UPDATE SET
    name = EXCLUDED.name,
    description = EXCLUDED.description,
    department_id = EXCLUDED.department_id,
    is_accredited = EXCLUDED.is_accredited,
    program_type = EXCLUDED.program_type,
    cdl_class = EXCLUDED.cdl_class,
    endorsements = EXCLUDED.endorsements,
    training_hours_classroom = EXCLUDED.training_hours_classroom,
    training_hours_range = EXCLUDED.training_hours_range,
    training_hours_road = EXCLUDED.training_hours_road,
    updated_at = CURRENT_TIMESTAMP;

-- Class A Doubles/Triples Endorsement (T)
INSERT INTO curriculum (
    code, name, description, department_id, degree_type,
    credit_hours, duration_semesters, is_active, is_online,
    is_accredited, program_type, cdl_class, endorsements,
    training_hours_classroom, training_hours_range, training_hours_road
)
VALUES
    ('cdl_class_a_doubles_triples', 'Class A CDL - Doubles/Triples Endorsement (T)',
     'Non-accredited endorsement training for pulling double or triple trailers. The T endorsement allows drivers to pull two or three trailers behind a tractor. Includes specialized training for coupling/uncoupling multiple trailers, handling characteristics, and safety procedures. Requires Class A CDL or concurrent enrollment.',
     (SELECT id FROM department WHERE code = 'CDL'), 'certificate',
     40, 1, TRUE, FALSE, FALSE, 'vocational', 'A', '["T"]',
     8, 16, 16)
ON CONFLICT (code) DO UPDATE SET
    name = EXCLUDED.name,
    description = EXCLUDED.description,
    department_id = EXCLUDED.department_id,
    is_accredited = EXCLUDED.is_accredited,
    program_type = EXCLUDED.program_type,
    cdl_class = EXCLUDED.cdl_class,
    endorsements = EXCLUDED.endorsements,
    training_hours_classroom = EXCLUDED.training_hours_classroom,
    training_hours_range = EXCLUDED.training_hours_range,
    training_hours_road = EXCLUDED.training_hours_road,
    updated_at = CURRENT_TIMESTAMP;

-- Class A Tanker Endorsement (N)
INSERT INTO curriculum (
    code, name, description, department_id, degree_type,
    credit_hours, duration_semesters, is_active, is_online,
    is_accredited, program_type, cdl_class, endorsements,
    training_hours_classroom, training_hours_range, training_hours_road
)
VALUES
    ('cdl_class_a_tanker', 'Class A CDL - Tanker Endorsement (N)',
     'Non-accredited endorsement training for transporting liquid or gaseous materials in bulk tanks. The N endorsement is required for any tank vehicle with a capacity of 1,000 gallons or more. Includes training on tank dynamics, surge, loading/unloading procedures, and emergency response. Requires Class A CDL or concurrent enrollment.',
     (SELECT id FROM department WHERE code = 'CDL'), 'certificate',
     40, 1, TRUE, FALSE, FALSE, 'vocational', 'A', '["N"]',
     12, 14, 14)
ON CONFLICT (code) DO UPDATE SET
    name = EXCLUDED.name,
    description = EXCLUDED.description,
    department_id = EXCLUDED.department_id,
    is_accredited = EXCLUDED.is_accredited,
    program_type = EXCLUDED.program_type,
    cdl_class = EXCLUDED.cdl_class,
    endorsements = EXCLUDED.endorsements,
    training_hours_classroom = EXCLUDED.training_hours_classroom,
    training_hours_range = EXCLUDED.training_hours_range,
    training_hours_road = EXCLUDED.training_hours_road,
    updated_at = CURRENT_TIMESTAMP;

-- Class A Hazmat Endorsement (H)
INSERT INTO curriculum (
    code, name, description, department_id, degree_type,
    credit_hours, duration_semesters, is_active, is_online,
    is_accredited, program_type, cdl_class, endorsements,
    training_hours_classroom, training_hours_range, training_hours_road
)
VALUES
    ('cdl_class_a_hazmat', 'Class A CDL - Hazardous Materials Endorsement (H)',
     'Non-accredited endorsement training for transporting hazardous materials. The H endorsement requires TSA background check and allows transport of hazardous materials as defined in 49 CFR 383.5. Includes hazmat classification, placarding, documentation, emergency procedures, and security awareness. Can be combined with N endorsement for X (tanker with hazmat). Requires Class A CDL or concurrent enrollment.',
     (SELECT id FROM department WHERE code = 'CDL'), 'certificate',
     32, 1, TRUE, FALSE, FALSE, 'vocational', 'A', '["H"]',
     24, 4, 4)
ON CONFLICT (code) DO UPDATE SET
    name = EXCLUDED.name,
    description = EXCLUDED.description,
    department_id = EXCLUDED.department_id,
    is_accredited = EXCLUDED.is_accredited,
    program_type = EXCLUDED.program_type,
    cdl_class = EXCLUDED.cdl_class,
    endorsements = EXCLUDED.endorsements,
    training_hours_classroom = EXCLUDED.training_hours_classroom,
    training_hours_range = EXCLUDED.training_hours_range,
    training_hours_road = EXCLUDED.training_hours_road,
    updated_at = CURRENT_TIMESTAMP;

-- Class A Air Brakes Training
INSERT INTO curriculum (
    code, name, description, department_id, degree_type,
    credit_hours, duration_semesters, is_active, is_online,
    is_accredited, program_type, cdl_class, endorsements,
    training_hours_classroom, training_hours_range, training_hours_road
)
VALUES
    ('cdl_class_a_air_brakes', 'Class A CDL - Air Brakes Training',
     'Non-accredited training to remove the Air Brake restriction from a Class A CDL. Without this training, CDL holders are restricted to vehicles without air brakes. Covers air brake system components, dual air brake systems, inspecting air brakes, and using air brakes properly. Includes both knowledge and skills testing preparation.',
     (SELECT id FROM department WHERE code = 'CDL'), 'certificate',
     24, 1, TRUE, FALSE, FALSE, 'vocational', 'A', '["air_brakes"]',
     8, 8, 8)
ON CONFLICT (code) DO UPDATE SET
    name = EXCLUDED.name,
    description = EXCLUDED.description,
    department_id = EXCLUDED.department_id,
    is_accredited = EXCLUDED.is_accredited,
    program_type = EXCLUDED.program_type,
    cdl_class = EXCLUDED.cdl_class,
    endorsements = EXCLUDED.endorsements,
    training_hours_classroom = EXCLUDED.training_hours_classroom,
    training_hours_range = EXCLUDED.training_hours_range,
    training_hours_road = EXCLUDED.training_hours_road,
    updated_at = CURRENT_TIMESTAMP;

-- =====================================================
-- CURRICULUM - Class B CDL Programs
-- =====================================================

-- Base Class B CDL Training
INSERT INTO curriculum (
    code, name, description, department_id, degree_type,
    credit_hours, duration_semesters, is_active, is_online,
    is_accredited, program_type, cdl_class, endorsements,
    training_hours_classroom, training_hours_range, training_hours_road
)
VALUES
    ('cdl_class_b', 'Class B CDL Training Program',
     'Non-accredited vocational training for Class B Commercial Driver''s License. Prepares students to operate single vehicles with a Gross Vehicle Weight Rating (GVWR) over 26,001 lbs, or tow vehicles under 10,000 lbs GVWR. Includes straight trucks, large buses, segmented buses, box trucks, and dump trucks. This is the base Class B program required before adding endorsements.',
     (SELECT id FROM department WHERE code = 'CDL'), 'certificate',
     120, 1, TRUE, FALSE, FALSE, 'vocational', 'B', '[]',
     30, 45, 45)
ON CONFLICT (code) DO UPDATE SET
    name = EXCLUDED.name,
    description = EXCLUDED.description,
    department_id = EXCLUDED.department_id,
    is_accredited = EXCLUDED.is_accredited,
    program_type = EXCLUDED.program_type,
    cdl_class = EXCLUDED.cdl_class,
    endorsements = EXCLUDED.endorsements,
    training_hours_classroom = EXCLUDED.training_hours_classroom,
    training_hours_range = EXCLUDED.training_hours_range,
    training_hours_road = EXCLUDED.training_hours_road,
    updated_at = CURRENT_TIMESTAMP;

-- Class B Passenger Endorsement (P)
INSERT INTO curriculum (
    code, name, description, department_id, degree_type,
    credit_hours, duration_semesters, is_active, is_online,
    is_accredited, program_type, cdl_class, endorsements,
    training_hours_classroom, training_hours_range, training_hours_road
)
VALUES
    ('cdl_class_b_passenger', 'Class B CDL - Passenger Endorsement (P)',
     'Non-accredited endorsement training for operating vehicles designed to transport 16 or more passengers including the driver. The P endorsement covers transit buses, motor coaches, and shuttle buses. Includes passenger safety, loading/unloading procedures, ADA compliance, emergency evacuation, and passenger management. Requires Class B CDL or concurrent enrollment.',
     (SELECT id FROM department WHERE code = 'CDL'), 'certificate',
     40, 1, TRUE, FALSE, FALSE, 'vocational', 'B', '["P"]',
     12, 14, 14)
ON CONFLICT (code) DO UPDATE SET
    name = EXCLUDED.name,
    description = EXCLUDED.description,
    department_id = EXCLUDED.department_id,
    is_accredited = EXCLUDED.is_accredited,
    program_type = EXCLUDED.program_type,
    cdl_class = EXCLUDED.cdl_class,
    endorsements = EXCLUDED.endorsements,
    training_hours_classroom = EXCLUDED.training_hours_classroom,
    training_hours_range = EXCLUDED.training_hours_range,
    training_hours_road = EXCLUDED.training_hours_road,
    updated_at = CURRENT_TIMESTAMP;

-- Class B School Bus Endorsement (S)
INSERT INTO curriculum (
    code, name, description, department_id, degree_type,
    credit_hours, duration_semesters, is_active, is_online,
    is_accredited, program_type, cdl_class, endorsements,
    training_hours_classroom, training_hours_range, training_hours_road
)
VALUES
    ('cdl_class_b_school_bus', 'Class B CDL - School Bus Endorsement (S)',
     'Non-accredited endorsement training for operating school buses. The S endorsement requires P endorsement and covers school bus-specific operations. Includes student management, school bus laws, loading/unloading at schools and bus stops, railroad crossing procedures, emergency evacuation drills, and special needs transportation. Requires Class B CDL with P endorsement or concurrent enrollment.',
     (SELECT id FROM department WHERE code = 'CDL'), 'certificate',
     40, 1, TRUE, FALSE, FALSE, 'vocational', 'B', '["S"]',
     16, 12, 12)
ON CONFLICT (code) DO UPDATE SET
    name = EXCLUDED.name,
    description = EXCLUDED.description,
    department_id = EXCLUDED.department_id,
    is_accredited = EXCLUDED.is_accredited,
    program_type = EXCLUDED.program_type,
    cdl_class = EXCLUDED.cdl_class,
    endorsements = EXCLUDED.endorsements,
    training_hours_classroom = EXCLUDED.training_hours_classroom,
    training_hours_range = EXCLUDED.training_hours_range,
    training_hours_road = EXCLUDED.training_hours_road,
    updated_at = CURRENT_TIMESTAMP;

-- Class B Air Brakes Training
INSERT INTO curriculum (
    code, name, description, department_id, degree_type,
    credit_hours, duration_semesters, is_active, is_online,
    is_accredited, program_type, cdl_class, endorsements,
    training_hours_classroom, training_hours_range, training_hours_road
)
VALUES
    ('cdl_class_b_air_brakes', 'Class B CDL - Air Brakes Training',
     'Non-accredited training to remove the Air Brake restriction from a Class B CDL. Without this training, CDL holders are restricted to vehicles without air brakes. Covers air brake system components, inspecting air brakes, and using air brakes properly. Many Class B vehicles including buses and large trucks use air brakes.',
     (SELECT id FROM department WHERE code = 'CDL'), 'certificate',
     24, 1, TRUE, FALSE, FALSE, 'vocational', 'B', '["air_brakes"]',
     8, 8, 8)
ON CONFLICT (code) DO UPDATE SET
    name = EXCLUDED.name,
    description = EXCLUDED.description,
    department_id = EXCLUDED.department_id,
    is_accredited = EXCLUDED.is_accredited,
    program_type = EXCLUDED.program_type,
    cdl_class = EXCLUDED.cdl_class,
    endorsements = EXCLUDED.endorsements,
    training_hours_classroom = EXCLUDED.training_hours_classroom,
    training_hours_range = EXCLUDED.training_hours_range,
    training_hours_road = EXCLUDED.training_hours_road,
    updated_at = CURRENT_TIMESTAMP;

-- =====================================================
-- CURRICULUM PREREQUISITES - CDL Programs
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
-- CURRICULUM FORM REQUIREMENTS - CDL Programs
-- =====================================================

-- All CDL programs require: personal_info, emergency_contact, medical_info, consent
-- Base programs and most endorsements also require documents

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

-- Log the seeding
DO $$
BEGIN
    RAISE NOTICE 'Vocational CDL curriculum seeding completed successfully';
    RAISE NOTICE 'Departments: 1, Curriculums: 9 (5 Class A, 4 Class B)';
END $$;

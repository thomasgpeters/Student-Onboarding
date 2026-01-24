-- =============================================================================
-- Student Onboarding System - Seed Data
-- =============================================================================
-- Comprehensive seed data for testing and development including:
-- - Admin users (admin_user table)
-- - Instructor records and qualifications (if migration 014 is applied)
-- - Student records with enrollment data
--
-- Prerequisites:
--   1. Run schema.sql first
--   2. Run install.sql
--   3. Run switch_to_vocational.sql (for CDL curriculum)
--   4. OPTIONAL: Run migrations/014_add_instructor_feature.sql (for instructor data)
--
-- Usage:
--   psql -U postgres -d student_onboarding -f database/seed_data.sql
-- =============================================================================

-- Password hash for all seed users: 'Password123!'
-- bcrypt hash generated with cost factor 12
-- In production, change these passwords immediately!

-- =====================================================
-- SECTION 1: ADMIN USERS (Legacy admin_user table)
-- =====================================================
-- Creates admin users in the admin_user table
-- Roles: admin, instructor

DO $$
DECLARE
    v_dept_cdl INTEGER;
    v_admin_id INTEGER;
BEGIN
    SELECT id INTO v_dept_cdl FROM department WHERE code = 'CDL';

    RAISE NOTICE '=== Creating Admin Users ===';

    -- Super Admin: Director of Operations
    IF NOT EXISTS (SELECT 1 FROM admin_user WHERE email = 'director@cdlschool.edu') THEN
        INSERT INTO admin_user (email, password_hash, first_name, last_name, role, department_id, is_active)
        VALUES ('director@cdlschool.edu', '$2a$12$LQv3c1yqBWVHxkd0LHAkCOYz6TtxMQJqhN8/X4bHCE5C1YXF.Wxbi',
                'Margaret', 'Reynolds', 'admin', v_dept_cdl, TRUE);
        RAISE NOTICE 'Created admin: director@cdlschool.edu';
    END IF;

    -- Manager Admin: Program Manager
    IF NOT EXISTS (SELECT 1 FROM admin_user WHERE email = 'manager@cdlschool.edu') THEN
        INSERT INTO admin_user (email, password_hash, first_name, last_name, role, department_id, is_active)
        VALUES ('manager@cdlschool.edu', '$2a$12$LQv3c1yqBWVHxkd0LHAkCOYz6TtxMQJqhN8/X4bHCE5C1YXF.Wxbi',
                'Thomas', 'Garcia', 'admin', v_dept_cdl, TRUE);
        RAISE NOTICE 'Created admin: manager@cdlschool.edu';
    END IF;

    -- Staff Admin: Admissions Coordinator
    IF NOT EXISTS (SELECT 1 FROM admin_user WHERE email = 'admissions@cdlschool.edu') THEN
        INSERT INTO admin_user (email, password_hash, first_name, last_name, role, department_id, is_active)
        VALUES ('admissions@cdlschool.edu', '$2a$12$LQv3c1yqBWVHxkd0LHAkCOYz6TtxMQJqhN8/X4bHCE5C1YXF.Wxbi',
                'Jennifer', 'Martinez', 'admin', v_dept_cdl, TRUE);
        RAISE NOTICE 'Created admin: admissions@cdlschool.edu';
    END IF;

    RAISE NOTICE '=== Admin Users Created ===';
END $$;

-- =====================================================
-- SECTION 2: INSTRUCTOR USERS
-- =====================================================
-- Creates instructor records in admin_user table
-- If migration 014 is applied, also creates instructor details

DO $$
DECLARE
    v_dept_cdl INTEGER;
    v_admin_user_id INTEGER;
    v_instructor_id INTEGER;
    v_has_instructor_table BOOLEAN;
BEGIN
    SELECT id INTO v_dept_cdl FROM department WHERE code = 'CDL';

    -- Check if instructor table exists (migration 014)
    SELECT EXISTS (
        SELECT FROM information_schema.tables
        WHERE table_schema = 'public' AND table_name = 'instructor'
    ) INTO v_has_instructor_table;

    RAISE NOTICE '=== Creating Instructor Users ===';
    IF v_has_instructor_table THEN
        RAISE NOTICE 'Instructor table found - will create full instructor records';
    ELSE
        RAISE NOTICE 'Instructor table not found - creating admin_user records only';
    END IF;

    -- =========================================================
    -- INSTRUCTOR 1: James Williams (Senior Instructor + Examiner)
    -- =========================================================
    IF NOT EXISTS (SELECT 1 FROM admin_user WHERE email = 'j.williams@cdlschool.edu') THEN
        INSERT INTO admin_user (email, password_hash, first_name, last_name, role, department_id, is_active)
        VALUES ('j.williams@cdlschool.edu', '$2a$12$LQv3c1yqBWVHxkd0LHAkCOYz6TtxMQJqhN8/X4bHCE5C1YXF.Wxbi',
                'James', 'Williams', 'instructor', v_dept_cdl, TRUE)
        RETURNING id INTO v_admin_user_id;

        IF v_has_instructor_table THEN
            INSERT INTO instructor (admin_user_id, employee_id, hire_date, instructor_type,
                                   cdl_number, cdl_class, cdl_state, cdl_expiration,
                                   is_certified_examiner, examiner_certification_number,
                                   examiner_certification_expiration, examiner_issuing_authority,
                                   endorsed_classes, endorsed_endorsements, can_schedule, can_validate, can_issue_cdl)
            VALUES (v_admin_user_id, 'INS001', '2018-03-15', 'both',
                    'CDL-A-12345678', 'A', 'TX', '2027-03-15',
                    TRUE, 'EX-TX-98765', '2026-12-31', 'Texas DPS',
                    '["A", "B"]', '["H", "N", "T", "P"]', TRUE, TRUE, TRUE)
            RETURNING id INTO v_instructor_id;

            -- Qualifications
            INSERT INTO instructor_qualification (instructor_id, qualification_type, qualification_name,
                                                 issuing_authority, certification_number, issued_date, expiration_date,
                                                 is_active, is_verified)
            VALUES
                (v_instructor_id, 'CDL_EXAMINER', 'CDL Third Party Examiner', 'Texas DPS', 'EX-TX-98765', '2020-01-01', '2026-12-31', TRUE, TRUE),
                (v_instructor_id, 'HAZMAT_INSTRUCTOR', 'HazMat Instructor Certification', 'DOT', 'HMI-2021-4567', '2021-06-15', '2026-06-15', TRUE, TRUE);
        END IF;

        RAISE NOTICE 'Created instructor: j.williams@cdlschool.edu (Senior + Examiner)';
    END IF;

    -- =========================================================
    -- INSTRUCTOR 2: Michael Johnson (Class A Instructor)
    -- =========================================================
    IF NOT EXISTS (SELECT 1 FROM admin_user WHERE email = 'm.johnson@cdlschool.edu') THEN
        INSERT INTO admin_user (email, password_hash, first_name, last_name, role, department_id, is_active)
        VALUES ('m.johnson@cdlschool.edu', '$2a$12$LQv3c1yqBWVHxkd0LHAkCOYz6TtxMQJqhN8/X4bHCE5C1YXF.Wxbi',
                'Michael', 'Johnson', 'instructor', v_dept_cdl, TRUE)
        RETURNING id INTO v_admin_user_id;

        IF v_has_instructor_table THEN
            INSERT INTO instructor (admin_user_id, employee_id, hire_date, instructor_type,
                                   cdl_number, cdl_class, cdl_state, cdl_expiration,
                                   is_certified_examiner, endorsed_classes, endorsed_endorsements,
                                   can_schedule, can_validate, can_issue_cdl)
            VALUES (v_admin_user_id, 'INS002', '2020-06-01', 'instructor',
                    'CDL-A-23456789', 'A', 'TX', '2026-08-20',
                    FALSE, '["A"]', '["N", "T"]', TRUE, TRUE, FALSE);
        END IF;

        RAISE NOTICE 'Created instructor: m.johnson@cdlschool.edu (Class A)';
    END IF;

    -- =========================================================
    -- INSTRUCTOR 3: Sarah Davis (Class B Specialist)
    -- =========================================================
    IF NOT EXISTS (SELECT 1 FROM admin_user WHERE email = 's.davis@cdlschool.edu') THEN
        INSERT INTO admin_user (email, password_hash, first_name, last_name, role, department_id, is_active)
        VALUES ('s.davis@cdlschool.edu', '$2a$12$LQv3c1yqBWVHxkd0LHAkCOYz6TtxMQJqhN8/X4bHCE5C1YXF.Wxbi',
                'Sarah', 'Davis', 'instructor', v_dept_cdl, TRUE)
        RETURNING id INTO v_admin_user_id;

        IF v_has_instructor_table THEN
            INSERT INTO instructor (admin_user_id, employee_id, hire_date, instructor_type,
                                   cdl_number, cdl_class, cdl_state, cdl_expiration,
                                   is_certified_examiner, endorsed_classes, endorsed_endorsements,
                                   can_schedule, can_validate, can_issue_cdl)
            VALUES (v_admin_user_id, 'INS003', '2019-09-01', 'instructor',
                    'CDL-B-34567890', 'B', 'TX', '2027-01-10',
                    FALSE, '["B"]', '["P", "S"]', TRUE, TRUE, FALSE);
        END IF;

        RAISE NOTICE 'Created instructor: s.davis@cdlschool.edu (Class B)';
    END IF;

    -- =========================================================
    -- INSTRUCTOR 4: Robert Thompson (Examiner Only)
    -- =========================================================
    IF NOT EXISTS (SELECT 1 FROM admin_user WHERE email = 'r.thompson@cdlschool.edu') THEN
        INSERT INTO admin_user (email, password_hash, first_name, last_name, role, department_id, is_active)
        VALUES ('r.thompson@cdlschool.edu', '$2a$12$LQv3c1yqBWVHxkd0LHAkCOYz6TtxMQJqhN8/X4bHCE5C1YXF.Wxbi',
                'Robert', 'Thompson', 'instructor', v_dept_cdl, TRUE)
        RETURNING id INTO v_admin_user_id;

        IF v_has_instructor_table THEN
            INSERT INTO instructor (admin_user_id, employee_id, hire_date, instructor_type,
                                   cdl_number, cdl_class, cdl_state, cdl_expiration,
                                   is_certified_examiner, examiner_certification_number,
                                   examiner_certification_expiration, examiner_issuing_authority,
                                   endorsed_classes, endorsed_endorsements,
                                   can_schedule, can_validate, can_issue_cdl)
            VALUES (v_admin_user_id, 'INS004', '2017-01-15', 'examiner',
                    'CDL-A-45678901', 'A', 'TX', '2026-05-30',
                    TRUE, 'EX-TX-11111', '2027-06-30', 'Texas DPS',
                    '["A", "B"]', '["H", "N", "T", "P", "S"]', FALSE, TRUE, TRUE)
            RETURNING id INTO v_instructor_id;

            INSERT INTO instructor_qualification (instructor_id, qualification_type, qualification_name,
                                                 issuing_authority, certification_number, issued_date, expiration_date,
                                                 is_active, is_verified)
            VALUES (v_instructor_id, 'CDL_EXAMINER', 'CDL Third Party Examiner', 'Texas DPS', 'EX-TX-11111', '2019-07-01', '2027-06-30', TRUE, TRUE);
        END IF;

        RAISE NOTICE 'Created examiner: r.thompson@cdlschool.edu';
    END IF;

    RAISE NOTICE '=== Instructor Users Created ===';
END $$;

-- =====================================================
-- SECTION 3: INSTRUCTOR AVAILABILITY
-- =====================================================
-- Only runs if migration 014 has been applied

DO $$
DECLARE
    v_instructor_williams INTEGER;
    v_instructor_johnson INTEGER;
    v_instructor_davis INTEGER;
    v_instructor_thompson INTEGER;
    v_has_availability_table BOOLEAN;
BEGIN
    -- Check if instructor_availability table exists
    SELECT EXISTS (
        SELECT FROM information_schema.tables
        WHERE table_schema = 'public' AND table_name = 'instructor_availability'
    ) INTO v_has_availability_table;

    IF NOT v_has_availability_table THEN
        RAISE NOTICE 'Skipping instructor availability - table does not exist (run migration 014)';
        RETURN;
    END IF;

    RAISE NOTICE '=== Creating Instructor Availability ===';

    SELECT id INTO v_instructor_williams FROM instructor WHERE employee_id = 'INS001';
    SELECT id INTO v_instructor_johnson FROM instructor WHERE employee_id = 'INS002';
    SELECT id INTO v_instructor_davis FROM instructor WHERE employee_id = 'INS003';
    SELECT id INTO v_instructor_thompson FROM instructor WHERE employee_id = 'INS004';

    -- Williams: Monday-Friday 7am-4pm
    IF v_instructor_williams IS NOT NULL THEN
        INSERT INTO instructor_availability (instructor_id, day_of_week, start_time, end_time, availability_type, preferred_location)
        VALUES
            (v_instructor_williams, 1, '07:00', '16:00', 'available', 'Main Campus'),
            (v_instructor_williams, 2, '07:00', '16:00', 'available', 'Main Campus'),
            (v_instructor_williams, 3, '07:00', '16:00', 'available', 'Main Campus'),
            (v_instructor_williams, 4, '07:00', '16:00', 'available', 'Main Campus'),
            (v_instructor_williams, 5, '07:00', '16:00', 'available', 'Main Campus')
        ON CONFLICT DO NOTHING;
        RAISE NOTICE 'Added availability for Williams (Mon-Fri 7am-4pm)';
    END IF;

    -- Johnson: Monday-Thursday 6am-3pm
    IF v_instructor_johnson IS NOT NULL THEN
        INSERT INTO instructor_availability (instructor_id, day_of_week, start_time, end_time, availability_type, preferred_location)
        VALUES
            (v_instructor_johnson, 1, '06:00', '15:00', 'available', 'Training Range'),
            (v_instructor_johnson, 2, '06:00', '15:00', 'available', 'Training Range'),
            (v_instructor_johnson, 3, '06:00', '15:00', 'available', 'Training Range'),
            (v_instructor_johnson, 4, '06:00', '15:00', 'available', 'Training Range')
        ON CONFLICT DO NOTHING;
        RAISE NOTICE 'Added availability for Johnson (Mon-Thu 6am-3pm)';
    END IF;

    -- Davis: Tuesday-Saturday 8am-5pm
    IF v_instructor_davis IS NOT NULL THEN
        INSERT INTO instructor_availability (instructor_id, day_of_week, start_time, end_time, availability_type, preferred_location)
        VALUES
            (v_instructor_davis, 2, '08:00', '17:00', 'available', 'Bus Depot'),
            (v_instructor_davis, 3, '08:00', '17:00', 'available', 'Bus Depot'),
            (v_instructor_davis, 4, '08:00', '17:00', 'available', 'Bus Depot'),
            (v_instructor_davis, 5, '08:00', '17:00', 'available', 'Bus Depot'),
            (v_instructor_davis, 6, '08:00', '17:00', 'available', 'Bus Depot')
        ON CONFLICT DO NOTHING;
        RAISE NOTICE 'Added availability for Davis (Tue-Sat 8am-5pm)';
    END IF;

    -- Thompson: Wednesday and Friday only (testing days)
    IF v_instructor_thompson IS NOT NULL THEN
        INSERT INTO instructor_availability (instructor_id, day_of_week, start_time, end_time, availability_type, preferred_location, notes)
        VALUES
            (v_instructor_thompson, 3, '08:00', '16:00', 'available', 'Testing Center', 'CDL Testing Day'),
            (v_instructor_thompson, 5, '08:00', '16:00', 'available', 'Testing Center', 'CDL Testing Day')
        ON CONFLICT DO NOTHING;
        RAISE NOTICE 'Added availability for Thompson (Wed/Fri 8am-4pm)';
    END IF;

    RAISE NOTICE '=== Instructor Availability Created ===';
END $$;

-- =====================================================
-- SECTION 4: STUDENT RECORDS
-- =====================================================
-- Creates students in the legacy student table

DO $$
DECLARE
    v_curriculum_class_a INTEGER;
    v_curriculum_class_b INTEGER;
    v_student_id INTEGER;
BEGIN
    SELECT id INTO v_curriculum_class_a FROM curriculum WHERE code = 'cdl_class_a';
    SELECT id INTO v_curriculum_class_b FROM curriculum WHERE code = 'cdl_class_b';

    RAISE NOTICE '=== Creating Student Records ===';

    -- Student 1: John Smith - Class A, veteran
    IF NOT EXISTS (SELECT 1 FROM student WHERE email = 'john.smith@email.com') THEN
        INSERT INTO student (email, password_hash, first_name, last_name, middle_name,
                            date_of_birth, gender, citizenship_status,
                            curriculum_id, enrollment_date, is_veteran)
        VALUES ('john.smith@email.com', '$2a$12$LQv3c1yqBWVHxkd0LHAkCOYz6TtxMQJqhN8/X4bHCE5C1YXF.Wxbi',
                'John', 'Smith', 'Robert',
                '1992-05-15', 'male', 'US Citizen',
                v_curriculum_class_a, '2026-01-06', TRUE);
        RAISE NOTICE 'Created student: john.smith@email.com (Class A, Veteran)';
    END IF;

    -- Student 2: Maria Garcia - Class A
    IF NOT EXISTS (SELECT 1 FROM student WHERE email = 'maria.garcia@email.com') THEN
        INSERT INTO student (email, password_hash, first_name, last_name,
                            date_of_birth, gender, citizenship_status,
                            curriculum_id, enrollment_date)
        VALUES ('maria.garcia@email.com', '$2a$12$LQv3c1yqBWVHxkd0LHAkCOYz6TtxMQJqhN8/X4bHCE5C1YXF.Wxbi',
                'Maria', 'Garcia',
                '1988-11-22', 'female', 'US Citizen',
                v_curriculum_class_a, '2025-12-02');
        RAISE NOTICE 'Created student: maria.garcia@email.com (Class A)';
    END IF;

    -- Student 3: David Lee - Class A
    IF NOT EXISTS (SELECT 1 FROM student WHERE email = 'david.lee@email.com') THEN
        INSERT INTO student (email, password_hash, first_name, last_name,
                            date_of_birth, gender, citizenship_status,
                            curriculum_id, enrollment_date)
        VALUES ('david.lee@email.com', '$2a$12$LQv3c1yqBWVHxkd0LHAkCOYz6TtxMQJqhN8/X4bHCE5C1YXF.Wxbi',
                'David', 'Lee',
                '1995-03-08', 'male', 'US Citizen',
                v_curriculum_class_a, '2025-11-04');
        RAISE NOTICE 'Created student: david.lee@email.com (Class A)';
    END IF;

    -- Student 4: Lisa Chen - Class B
    IF NOT EXISTS (SELECT 1 FROM student WHERE email = 'lisa.chen@email.com') THEN
        INSERT INTO student (email, password_hash, first_name, last_name,
                            date_of_birth, gender, citizenship_status,
                            curriculum_id, enrollment_date)
        VALUES ('lisa.chen@email.com', '$2a$12$LQv3c1yqBWVHxkd0LHAkCOYz6TtxMQJqhN8/X4bHCE5C1YXF.Wxbi',
                'Lisa', 'Chen',
                '1990-07-19', 'female', 'US Citizen',
                v_curriculum_class_b, '2026-01-06');
        RAISE NOTICE 'Created student: lisa.chen@email.com (Class B)';
    END IF;

    -- Student 5: Kevin Brown - Class B
    IF NOT EXISTS (SELECT 1 FROM student WHERE email = 'kevin.brown@email.com') THEN
        INSERT INTO student (email, password_hash, first_name, last_name,
                            date_of_birth, gender, citizenship_status,
                            curriculum_id, enrollment_date)
        VALUES ('kevin.brown@email.com', '$2a$12$LQv3c1yqBWVHxkd0LHAkCOYz6TtxMQJqhN8/X4bHCE5C1YXF.Wxbi',
                'Kevin', 'Brown',
                '1985-12-03', 'male', 'US Citizen',
                v_curriculum_class_b, '2025-10-07');
        RAISE NOTICE 'Created student: kevin.brown@email.com (Class B)';
    END IF;

    -- Student 6: Amanda Wilson - Class A
    IF NOT EXISTS (SELECT 1 FROM student WHERE email = 'amanda.wilson@email.com') THEN
        INSERT INTO student (email, password_hash, first_name, last_name,
                            date_of_birth, gender,
                            curriculum_id, enrollment_date)
        VALUES ('amanda.wilson@email.com', '$2a$12$LQv3c1yqBWVHxkd0LHAkCOYz6TtxMQJqhN8/X4bHCE5C1YXF.Wxbi',
                'Amanda', 'Wilson',
                '1998-09-25', 'female',
                v_curriculum_class_a, '2026-01-20');
        RAISE NOTICE 'Created student: amanda.wilson@email.com (Class A)';
    END IF;

    -- Student 7: Marcus Taylor - Class A
    IF NOT EXISTS (SELECT 1 FROM student WHERE email = 'marcus.taylor@email.com') THEN
        INSERT INTO student (email, password_hash, first_name, last_name,
                            date_of_birth, gender, citizenship_status,
                            curriculum_id, enrollment_date)
        VALUES ('marcus.taylor@email.com', '$2a$12$LQv3c1yqBWVHxkd0LHAkCOYz6TtxMQJqhN8/X4bHCE5C1YXF.Wxbi',
                'Marcus', 'Taylor',
                '1993-02-14', 'male', 'US Citizen',
                v_curriculum_class_a, '2026-01-06');
        RAISE NOTICE 'Created student: marcus.taylor@email.com (Class A)';
    END IF;

    -- Student 8: Carlos Rodriguez - Class A, permanent resident
    IF NOT EXISTS (SELECT 1 FROM student WHERE email = 'carlos.rodriguez@email.com') THEN
        INSERT INTO student (email, password_hash, first_name, last_name,
                            date_of_birth, gender, citizenship_status,
                            curriculum_id, enrollment_date)
        VALUES ('carlos.rodriguez@email.com', '$2a$12$LQv3c1yqBWVHxkd0LHAkCOYz6TtxMQJqhN8/X4bHCE5C1YXF.Wxbi',
                'Carlos', 'Rodriguez',
                '1991-08-30', 'male', 'Permanent Resident',
                v_curriculum_class_a, '2026-01-13');
        RAISE NOTICE 'Created student: carlos.rodriguez@email.com (Class A, Perm Resident)';
    END IF;

    RAISE NOTICE '=== Student Records Created ===';
END $$;

-- =====================================================
-- SECTION 5: STUDENT ADDRESSES
-- =====================================================

DO $$
DECLARE
    v_student_id INTEGER;
BEGIN
    RAISE NOTICE '=== Creating Student Addresses ===';

    -- John Smith
    SELECT id INTO v_student_id FROM student WHERE email = 'john.smith@email.com';
    IF v_student_id IS NOT NULL AND NOT EXISTS (SELECT 1 FROM student_address WHERE student_id = v_student_id) THEN
        INSERT INTO student_address (student_id, address_type, street1, city, state, postal_code, country, is_primary)
        VALUES (v_student_id, 'home', '123 Main Street', 'Dallas', 'TX', '75201', 'USA', TRUE);
    END IF;

    -- Maria Garcia
    SELECT id INTO v_student_id FROM student WHERE email = 'maria.garcia@email.com';
    IF v_student_id IS NOT NULL AND NOT EXISTS (SELECT 1 FROM student_address WHERE student_id = v_student_id) THEN
        INSERT INTO student_address (student_id, address_type, street1, city, state, postal_code, country, is_primary)
        VALUES (v_student_id, 'home', '456 Oak Avenue', 'Fort Worth', 'TX', '76102', 'USA', TRUE);
    END IF;

    -- David Lee
    SELECT id INTO v_student_id FROM student WHERE email = 'david.lee@email.com';
    IF v_student_id IS NOT NULL AND NOT EXISTS (SELECT 1 FROM student_address WHERE student_id = v_student_id) THEN
        INSERT INTO student_address (student_id, address_type, street1, city, state, postal_code, country, is_primary)
        VALUES (v_student_id, 'home', '789 Pine Road', 'Arlington', 'TX', '76010', 'USA', TRUE);
    END IF;

    -- Lisa Chen
    SELECT id INTO v_student_id FROM student WHERE email = 'lisa.chen@email.com';
    IF v_student_id IS NOT NULL AND NOT EXISTS (SELECT 1 FROM student_address WHERE student_id = v_student_id) THEN
        INSERT INTO student_address (student_id, address_type, street1, city, state, postal_code, country, is_primary)
        VALUES (v_student_id, 'home', '321 Elm Street', 'Plano', 'TX', '75023', 'USA', TRUE);
    END IF;

    -- Kevin Brown
    SELECT id INTO v_student_id FROM student WHERE email = 'kevin.brown@email.com';
    IF v_student_id IS NOT NULL AND NOT EXISTS (SELECT 1 FROM student_address WHERE student_id = v_student_id) THEN
        INSERT INTO student_address (student_id, address_type, street1, city, state, postal_code, country, is_primary)
        VALUES (v_student_id, 'home', '555 Cedar Lane', 'Irving', 'TX', '75038', 'USA', TRUE);
    END IF;

    -- Amanda Wilson
    SELECT id INTO v_student_id FROM student WHERE email = 'amanda.wilson@email.com';
    IF v_student_id IS NOT NULL AND NOT EXISTS (SELECT 1 FROM student_address WHERE student_id = v_student_id) THEN
        INSERT INTO student_address (student_id, address_type, street1, city, state, postal_code, country, is_primary)
        VALUES (v_student_id, 'home', '777 Birch Drive', 'Garland', 'TX', '75040', 'USA', TRUE);
    END IF;

    -- Marcus Taylor
    SELECT id INTO v_student_id FROM student WHERE email = 'marcus.taylor@email.com';
    IF v_student_id IS NOT NULL AND NOT EXISTS (SELECT 1 FROM student_address WHERE student_id = v_student_id) THEN
        INSERT INTO student_address (student_id, address_type, street1, city, state, postal_code, country, is_primary)
        VALUES (v_student_id, 'home', '888 Maple Court', 'Mesquite', 'TX', '75149', 'USA', TRUE);
    END IF;

    -- Carlos Rodriguez
    SELECT id INTO v_student_id FROM student WHERE email = 'carlos.rodriguez@email.com';
    IF v_student_id IS NOT NULL AND NOT EXISTS (SELECT 1 FROM student_address WHERE student_id = v_student_id) THEN
        INSERT INTO student_address (student_id, address_type, street1, city, state, postal_code, country, is_primary)
        VALUES (v_student_id, 'home', '999 Walnut Street', 'Richardson', 'TX', '75080', 'USA', TRUE);
    END IF;

    RAISE NOTICE '=== Student Addresses Created ===';
END $$;

-- =====================================================
-- SECTION 6: STUDENT ENDORSEMENTS (SKIPPED)
-- =====================================================
-- Note: The student_endorsement table tracks curriculum enrollments,
-- not individual CDL endorsement codes. Students are already enrolled
-- in their primary curriculum via the student.curriculum_id field.

DO $$
BEGIN
    RAISE NOTICE '=== Skipping Student Endorsements (tracked via curriculum) ===';
END $$;

-- =====================================================
-- VERIFICATION & SUMMARY
-- =====================================================

DO $$
DECLARE
    admin_count INTEGER;
    instructor_count INTEGER;
    student_count INTEGER;
    address_count INTEGER;
    v_has_instructor_table BOOLEAN;
    v_instructor_detail_count INTEGER;
    v_availability_count INTEGER;
BEGIN
    SELECT COUNT(*) INTO admin_count FROM admin_user WHERE role = 'admin';
    SELECT COUNT(*) INTO instructor_count FROM admin_user WHERE role = 'instructor';
    SELECT COUNT(*) INTO student_count FROM student;
    SELECT COUNT(*) INTO address_count FROM student_address;

    SELECT EXISTS (
        SELECT FROM information_schema.tables
        WHERE table_schema = 'public' AND table_name = 'instructor'
    ) INTO v_has_instructor_table;

    IF v_has_instructor_table THEN
        SELECT COUNT(*) INTO v_instructor_detail_count FROM instructor;
        SELECT COUNT(*) INTO v_availability_count FROM instructor_availability;
    END IF;

    RAISE NOTICE '';
    RAISE NOTICE '==============================================';
    RAISE NOTICE 'Seed Data Installation Complete!';
    RAISE NOTICE '==============================================';
    RAISE NOTICE '';
    RAISE NOTICE 'ADMIN USERS (admin_user table):';
    RAISE NOTICE '  - Admins: %', admin_count;
    RAISE NOTICE '  - Instructors: %', instructor_count;
    RAISE NOTICE '';
    IF v_has_instructor_table THEN
        RAISE NOTICE 'INSTRUCTOR DETAILS (migration 014 applied):';
        RAISE NOTICE '  - Instructor Records: %', v_instructor_detail_count;
        RAISE NOTICE '  - Availability Slots: %', v_availability_count;
        RAISE NOTICE '';
    END IF;
    RAISE NOTICE 'STUDENT DATA:';
    RAISE NOTICE '  - Students: %', student_count;
    RAISE NOTICE '  - Addresses: %', address_count;
    RAISE NOTICE '';
    RAISE NOTICE 'TEST CREDENTIALS (password: Password123!):';
    RAISE NOTICE '';
    RAISE NOTICE '  ADMINS:';
    RAISE NOTICE '    director@cdlschool.edu';
    RAISE NOTICE '    manager@cdlschool.edu';
    RAISE NOTICE '    admissions@cdlschool.edu';
    RAISE NOTICE '';
    RAISE NOTICE '  INSTRUCTORS:';
    RAISE NOTICE '    j.williams@cdlschool.edu (Class A, Senior + Examiner)';
    RAISE NOTICE '    m.johnson@cdlschool.edu  (Class A)';
    RAISE NOTICE '    s.davis@cdlschool.edu    (Class B)';
    RAISE NOTICE '    r.thompson@cdlschool.edu (Examiner Only)';
    RAISE NOTICE '';
    RAISE NOTICE '  STUDENTS:';
    RAISE NOTICE '    john.smith@email.com     (Class A, Veteran)';
    RAISE NOTICE '    maria.garcia@email.com   (Class A)';
    RAISE NOTICE '    david.lee@email.com      (Class A)';
    RAISE NOTICE '    lisa.chen@email.com      (Class B)';
    RAISE NOTICE '    kevin.brown@email.com    (Class B, Completed)';
    RAISE NOTICE '    amanda.wilson@email.com  (Class A, Pending)';
    RAISE NOTICE '    marcus.taylor@email.com  (Class A)';
    RAISE NOTICE '    carlos.rodriguez@email.com (Class A, Perm Resident)';
    RAISE NOTICE '';
    RAISE NOTICE '==============================================';
END $$;

-- =============================================================================
-- Student Onboarding System - Seed Data
-- =============================================================================
-- Comprehensive seed data for testing and development including:
-- - Users (app_user) with roles (admin, instructor, student)
-- - Admin profiles
-- - Instructor profiles and legacy instructor records
-- - Instructor assignments
-- - Instructor availability schedules
-- - Instructor qualifications
-- - Student profiles with enrollment data
--
-- Prerequisites:
--   1. Run schema.sql first
--   2. Run install.sql
--   3. Run switch_to_vocational.sql (for CDL curriculum)
--   4. Run migrations/014_add_instructor_feature.sql
--   5. Run migrations/015_unified_user_authentication.sql
--
-- Usage:
--   psql -U postgres -d student_onboarding -f database/seed_data.sql
-- =============================================================================

-- Password hash for all seed users: 'Password123!'
-- bcrypt hash generated with cost factor 12
-- In production, change these passwords immediately!

-- =====================================================
-- SECTION 1: ADMIN USERS
-- =====================================================
-- Creates admin users in the unified authentication system
-- Tables: app_user, user_roles, admin_profile

DO $$
DECLARE
    v_user_id INTEGER;
    v_dept_cdl INTEGER;
BEGIN
    SELECT id INTO v_dept_cdl FROM department WHERE code = 'CDL';

    RAISE NOTICE '=== Creating Admin Users ===';

    -- Super Admin: Director of Operations
    IF NOT EXISTS (SELECT 1 FROM app_user WHERE email = 'director@cdlschool.edu') THEN
        INSERT INTO app_user (email, password_hash, first_name, last_name, phone_number, is_active, login_enabled, email_verified)
        VALUES ('director@cdlschool.edu', '$2a$12$LQv3c1yqBWVHxkd0LHAkCOYz6TtxMQJqhN8/X4bHCE5C1YXF.Wxbi',
                'Margaret', 'Reynolds', '(555) 100-0001', TRUE, TRUE, TRUE)
        RETURNING id INTO v_user_id;

        INSERT INTO user_roles (user_id, role) VALUES (v_user_id, 'admin');
        INSERT INTO admin_profile (user_id, employee_id, department_id, admin_level,
                                   can_manage_students, can_manage_curriculum, can_manage_instructors,
                                   can_manage_admins, can_view_reports, can_revoke_access)
        VALUES (v_user_id, 'EMP001', v_dept_cdl, 'super_admin', TRUE, TRUE, TRUE, TRUE, TRUE, TRUE);
        RAISE NOTICE 'Created super admin: director@cdlschool.edu';
    END IF;

    -- Manager Admin: Program Manager
    IF NOT EXISTS (SELECT 1 FROM app_user WHERE email = 'manager@cdlschool.edu') THEN
        INSERT INTO app_user (email, password_hash, first_name, last_name, phone_number, is_active, login_enabled, email_verified)
        VALUES ('manager@cdlschool.edu', '$2a$12$LQv3c1yqBWVHxkd0LHAkCOYz6TtxMQJqhN8/X4bHCE5C1YXF.Wxbi',
                'Thomas', 'Garcia', '(555) 100-0002', TRUE, TRUE, TRUE)
        RETURNING id INTO v_user_id;

        INSERT INTO user_roles (user_id, role) VALUES (v_user_id, 'admin');
        INSERT INTO admin_profile (user_id, employee_id, department_id, admin_level,
                                   can_manage_students, can_manage_curriculum, can_manage_instructors,
                                   can_manage_admins, can_view_reports, can_revoke_access)
        VALUES (v_user_id, 'EMP002', v_dept_cdl, 'manager', TRUE, TRUE, TRUE, FALSE, TRUE, TRUE);
        RAISE NOTICE 'Created manager admin: manager@cdlschool.edu';
    END IF;

    -- Staff Admin: Admissions Coordinator
    IF NOT EXISTS (SELECT 1 FROM app_user WHERE email = 'admissions@cdlschool.edu') THEN
        INSERT INTO app_user (email, password_hash, first_name, last_name, phone_number, is_active, login_enabled, email_verified)
        VALUES ('admissions@cdlschool.edu', '$2a$12$LQv3c1yqBWVHxkd0LHAkCOYz6TtxMQJqhN8/X4bHCE5C1YXF.Wxbi',
                'Jennifer', 'Martinez', '(555) 100-0003', TRUE, TRUE, TRUE)
        RETURNING id INTO v_user_id;

        INSERT INTO user_roles (user_id, role) VALUES (v_user_id, 'admin');
        INSERT INTO admin_profile (user_id, employee_id, department_id, admin_level,
                                   can_manage_students, can_manage_curriculum, can_manage_instructors,
                                   can_manage_admins, can_view_reports, can_revoke_access)
        VALUES (v_user_id, 'EMP003', v_dept_cdl, 'staff', TRUE, FALSE, FALSE, FALSE, TRUE, FALSE);
        RAISE NOTICE 'Created staff admin: admissions@cdlschool.edu';
    END IF;

    RAISE NOTICE '=== Admin Users Created ===';
END $$;

-- =====================================================
-- SECTION 2: INSTRUCTOR USERS
-- =====================================================
-- Creates instructor users in both:
--   1. Unified auth system (app_user, user_roles, instructor_profile)
--   2. Legacy system (admin_user, instructor, instructor_qualification)
--
-- Instructor Types:
--   - instructor: Can teach and train students
--   - examiner: Can conduct official CDL tests and issue certifications
--   - both: Can perform all instructor and examiner functions
--
-- Capabilities:
--   - can_schedule: Can create/manage training sessions
--   - can_validate: Can validate student skill completion
--   - can_issue_cdl: Can issue official CDL certifications (examiners only)
--
-- Qualifications tracked:
--   - CDL license details (number, class, state, expiration)
--   - Examiner certifications
--   - Endorsed classes (A, B, C) and endorsements (H, N, T, P, S)

DO $$
DECLARE
    v_user_id INTEGER;
    v_admin_user_id INTEGER;
    v_instructor_id INTEGER;
    v_dept_cdl INTEGER;
BEGIN
    SELECT id INTO v_dept_cdl FROM department WHERE code = 'CDL';

    RAISE NOTICE '=== Creating Instructor Users ===';

    -- =========================================================
    -- INSTRUCTOR 1: James Williams (Senior Instructor + Examiner)
    -- =========================================================
    -- Role: Senior instructor and certified examiner
    -- CDL Class: A (can also teach B)
    -- Endorsements: H (Hazmat), N (Tanker), T (Doubles/Triples), P (Passenger)
    -- Capabilities: Full (schedule, validate, issue CDL)
    IF NOT EXISTS (SELECT 1 FROM app_user WHERE email = 'j.williams@cdlschool.edu') THEN
        -- Unified auth system
        INSERT INTO app_user (email, password_hash, first_name, last_name, phone_number, is_active, login_enabled, email_verified)
        VALUES ('j.williams@cdlschool.edu', '$2a$12$LQv3c1yqBWVHxkd0LHAkCOYz6TtxMQJqhN8/X4bHCE5C1YXF.Wxbi',
                'James', 'Williams', '(555) 200-0001', TRUE, TRUE, TRUE)
        RETURNING id INTO v_user_id;

        INSERT INTO user_roles (user_id, role) VALUES (v_user_id, 'instructor');
        INSERT INTO instructor_profile (user_id, employee_id, department_id, hire_date, instructor_type,
                                        cdl_number, cdl_class, cdl_expiration,
                                        is_certified_examiner, can_schedule, can_validate, can_issue_cdl)
        VALUES (v_user_id, 'INS001', v_dept_cdl, '2018-03-15', 'both',
                'CDL-A-12345678', 'A', '2027-03-15',
                TRUE, TRUE, TRUE, TRUE);

        -- Legacy system (for instructor_assignment compatibility)
        INSERT INTO admin_user (email, password_hash, first_name, last_name, role, department_id, is_active)
        VALUES ('j.williams@cdlschool.edu', '$2a$12$LQv3c1yqBWVHxkd0LHAkCOYz6TtxMQJqhN8/X4bHCE5C1YXF.Wxbi',
                'James', 'Williams', 'instructor', v_dept_cdl, TRUE)
        RETURNING id INTO v_admin_user_id;

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

        RAISE NOTICE 'Created senior instructor: j.williams@cdlschool.edu';
    END IF;

    -- =========================================================
    -- INSTRUCTOR 2: Michael Johnson (Class A Instructor)
    -- =========================================================
    -- Role: Standard instructor (no examiner certification)
    -- CDL Class: A
    -- Endorsements: N (Tanker), T (Doubles/Triples)
    -- Capabilities: Schedule and validate only (cannot issue CDL)
    IF NOT EXISTS (SELECT 1 FROM app_user WHERE email = 'm.johnson@cdlschool.edu') THEN
        INSERT INTO app_user (email, password_hash, first_name, last_name, phone_number, is_active, login_enabled, email_verified)
        VALUES ('m.johnson@cdlschool.edu', '$2a$12$LQv3c1yqBWVHxkd0LHAkCOYz6TtxMQJqhN8/X4bHCE5C1YXF.Wxbi',
                'Michael', 'Johnson', '(555) 200-0002', TRUE, TRUE, TRUE)
        RETURNING id INTO v_user_id;

        INSERT INTO user_roles (user_id, role) VALUES (v_user_id, 'instructor');
        INSERT INTO instructor_profile (user_id, employee_id, department_id, hire_date, instructor_type,
                                        cdl_number, cdl_class, cdl_expiration,
                                        is_certified_examiner, can_schedule, can_validate, can_issue_cdl)
        VALUES (v_user_id, 'INS002', v_dept_cdl, '2020-06-01', 'instructor',
                'CDL-A-23456789', 'A', '2026-08-20',
                FALSE, TRUE, TRUE, FALSE);

        -- Legacy system
        INSERT INTO admin_user (email, password_hash, first_name, last_name, role, department_id, is_active)
        VALUES ('m.johnson@cdlschool.edu', '$2a$12$LQv3c1yqBWVHxkd0LHAkCOYz6TtxMQJqhN8/X4bHCE5C1YXF.Wxbi',
                'Michael', 'Johnson', 'instructor', v_dept_cdl, TRUE)
        RETURNING id INTO v_admin_user_id;

        INSERT INTO instructor (admin_user_id, employee_id, hire_date, instructor_type,
                               cdl_number, cdl_class, cdl_state, cdl_expiration,
                               is_certified_examiner, endorsed_classes, endorsed_endorsements,
                               can_schedule, can_validate, can_issue_cdl)
        VALUES (v_admin_user_id, 'INS002', '2020-06-01', 'instructor',
                'CDL-A-23456789', 'A', 'TX', '2026-08-20',
                FALSE, '["A"]', '["N", "T"]', TRUE, TRUE, FALSE);

        RAISE NOTICE 'Created instructor: m.johnson@cdlschool.edu';
    END IF;

    -- =========================================================
    -- INSTRUCTOR 3: Sarah Davis (Class B Specialist)
    -- =========================================================
    -- Role: Standard instructor specializing in Class B (buses)
    -- CDL Class: B
    -- Endorsements: P (Passenger), S (School Bus)
    -- Capabilities: Schedule and validate only
    IF NOT EXISTS (SELECT 1 FROM app_user WHERE email = 's.davis@cdlschool.edu') THEN
        INSERT INTO app_user (email, password_hash, first_name, last_name, phone_number, is_active, login_enabled, email_verified)
        VALUES ('s.davis@cdlschool.edu', '$2a$12$LQv3c1yqBWVHxkd0LHAkCOYz6TtxMQJqhN8/X4bHCE5C1YXF.Wxbi',
                'Sarah', 'Davis', '(555) 200-0003', TRUE, TRUE, TRUE)
        RETURNING id INTO v_user_id;

        INSERT INTO user_roles (user_id, role) VALUES (v_user_id, 'instructor');
        INSERT INTO instructor_profile (user_id, employee_id, department_id, hire_date, instructor_type,
                                        cdl_number, cdl_class, cdl_expiration,
                                        is_certified_examiner, can_schedule, can_validate, can_issue_cdl)
        VALUES (v_user_id, 'INS003', v_dept_cdl, '2019-09-01', 'instructor',
                'CDL-B-34567890', 'B', '2027-01-10',
                FALSE, TRUE, TRUE, FALSE);

        -- Legacy system
        INSERT INTO admin_user (email, password_hash, first_name, last_name, role, department_id, is_active)
        VALUES ('s.davis@cdlschool.edu', '$2a$12$LQv3c1yqBWVHxkd0LHAkCOYz6TtxMQJqhN8/X4bHCE5C1YXF.Wxbi',
                'Sarah', 'Davis', 'instructor', v_dept_cdl, TRUE)
        RETURNING id INTO v_admin_user_id;

        INSERT INTO instructor (admin_user_id, employee_id, hire_date, instructor_type,
                               cdl_number, cdl_class, cdl_state, cdl_expiration,
                               is_certified_examiner, endorsed_classes, endorsed_endorsements,
                               can_schedule, can_validate, can_issue_cdl)
        VALUES (v_admin_user_id, 'INS003', '2019-09-01', 'instructor',
                'CDL-B-34567890', 'B', 'TX', '2027-01-10',
                FALSE, '["B"]', '["P", "S"]', TRUE, TRUE, FALSE);

        RAISE NOTICE 'Created instructor: s.davis@cdlschool.edu';
    END IF;

    -- =========================================================
    -- INSTRUCTOR 4: Robert Thompson (Examiner Only)
    -- =========================================================
    -- Role: Certified examiner only (does not teach)
    -- CDL Class: A (can also examine B)
    -- Endorsements: All (H, N, T, P, S)
    -- Capabilities: Validate and issue CDL only (no scheduling)
    IF NOT EXISTS (SELECT 1 FROM app_user WHERE email = 'r.thompson@cdlschool.edu') THEN
        INSERT INTO app_user (email, password_hash, first_name, last_name, phone_number, is_active, login_enabled, email_verified)
        VALUES ('r.thompson@cdlschool.edu', '$2a$12$LQv3c1yqBWVHxkd0LHAkCOYz6TtxMQJqhN8/X4bHCE5C1YXF.Wxbi',
                'Robert', 'Thompson', '(555) 200-0004', TRUE, TRUE, TRUE)
        RETURNING id INTO v_user_id;

        INSERT INTO user_roles (user_id, role) VALUES (v_user_id, 'instructor');
        INSERT INTO instructor_profile (user_id, employee_id, department_id, hire_date, instructor_type,
                                        cdl_number, cdl_class, cdl_expiration,
                                        is_certified_examiner, can_schedule, can_validate, can_issue_cdl)
        VALUES (v_user_id, 'INS004', v_dept_cdl, '2017-01-15', 'examiner',
                'CDL-A-45678901', 'A', '2026-05-30',
                TRUE, FALSE, TRUE, TRUE);

        -- Legacy system
        INSERT INTO admin_user (email, password_hash, first_name, last_name, role, department_id, is_active)
        VALUES ('r.thompson@cdlschool.edu', '$2a$12$LQv3c1yqBWVHxkd0LHAkCOYz6TtxMQJqhN8/X4bHCE5C1YXF.Wxbi',
                'Robert', 'Thompson', 'instructor', v_dept_cdl, TRUE)
        RETURNING id INTO v_admin_user_id;

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

        RAISE NOTICE 'Created examiner: r.thompson@cdlschool.edu';
    END IF;

    RAISE NOTICE '=== Instructor Users Created ===';
END $$;

-- =====================================================
-- SECTION 3: INSTRUCTOR AVAILABILITY
-- =====================================================
-- Defines recurring weekly availability for instructors
-- day_of_week: 0=Sunday, 1=Monday, ..., 6=Saturday

DO $$
DECLARE
    v_instructor_williams INTEGER;
    v_instructor_johnson INTEGER;
    v_instructor_davis INTEGER;
    v_instructor_thompson INTEGER;
BEGIN
    RAISE NOTICE '=== Creating Instructor Availability ===';

    SELECT id INTO v_instructor_williams FROM instructor WHERE employee_id = 'INS001';
    SELECT id INTO v_instructor_johnson FROM instructor WHERE employee_id = 'INS002';
    SELECT id INTO v_instructor_davis FROM instructor WHERE employee_id = 'INS003';
    SELECT id INTO v_instructor_thompson FROM instructor WHERE employee_id = 'INS004';

    -- Williams: Monday-Friday 7am-4pm (full-time senior instructor)
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

    -- Johnson: Monday-Thursday 6am-3pm (early shift for range training)
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

    -- Davis: Tuesday-Saturday 8am-5pm (bus program, weekend availability)
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

    -- Thompson (Examiner): Wednesday and Friday only - designated testing days
    IF v_instructor_thompson IS NOT NULL THEN
        INSERT INTO instructor_availability (instructor_id, day_of_week, start_time, end_time, availability_type, preferred_location, notes)
        VALUES
            (v_instructor_thompson, 3, '08:00', '16:00', 'available', 'Testing Center', 'CDL Testing Day'),
            (v_instructor_thompson, 5, '08:00', '16:00', 'available', 'Testing Center', 'CDL Testing Day')
        ON CONFLICT DO NOTHING;
        RAISE NOTICE 'Added availability for Thompson (Wed/Fri 8am-4pm - Testing)';
    END IF;

    RAISE NOTICE '=== Instructor Availability Created ===';
END $$;

-- =====================================================
-- SECTION 4: STUDENT USERS
-- =====================================================
-- Creates student users in the unified authentication system
-- Tables: app_user, user_roles, student_profile
-- Note: Uses new unified system only (not legacy student table)

DO $$
DECLARE
    v_user_id INTEGER;
    v_curriculum_class_a INTEGER;
    v_curriculum_class_b INTEGER;
BEGIN
    SELECT id INTO v_curriculum_class_a FROM curriculum WHERE code = 'cdl_class_a';
    SELECT id INTO v_curriculum_class_b FROM curriculum WHERE code = 'cdl_class_b';

    RAISE NOTICE '=== Creating Student Users ===';

    -- Student 1: John Smith - Class A, veteran, completed intake
    IF NOT EXISTS (SELECT 1 FROM app_user WHERE email = 'john.smith@email.com') THEN
        INSERT INTO app_user (email, password_hash, first_name, last_name, middle_name, phone_number, is_active, login_enabled, email_verified)
        VALUES ('john.smith@email.com', '$2a$12$LQv3c1yqBWVHxkd0LHAkCOYz6TtxMQJqhN8/X4bHCE5C1YXF.Wxbi',
                'John', 'Smith', 'Robert', '(555) 301-0001', TRUE, TRUE, TRUE)
        RETURNING id INTO v_user_id;

        INSERT INTO user_roles (user_id, role) VALUES (v_user_id, 'student');
        INSERT INTO student_profile (user_id, preferred_pronouns, date_of_birth, gender, alternate_phone,
                                     address_line1, city, state, zip_code,
                                     citizenship_status, curriculum_id, student_type,
                                     enrollment_date, expected_graduation,
                                     is_veteran, intake_status)
        VALUES (v_user_id, 'he/him', '1992-05-15', 'male', '(555) 301-0002',
                '123 Main Street', 'Dallas', 'TX', '75201',
                'US Citizen', v_curriculum_class_a, 'full_time',
                '2026-01-06', '2026-02-03',
                TRUE, 'completed');
        RAISE NOTICE 'Created student: john.smith@email.com (Class A, Veteran)';
    END IF;

    -- Student 2: Maria Garcia - Class A, pursuing Tanker endorsement
    IF NOT EXISTS (SELECT 1 FROM app_user WHERE email = 'maria.garcia@email.com') THEN
        INSERT INTO app_user (email, password_hash, first_name, last_name, phone_number, is_active, login_enabled, email_verified)
        VALUES ('maria.garcia@email.com', '$2a$12$LQv3c1yqBWVHxkd0LHAkCOYz6TtxMQJqhN8/X4bHCE5C1YXF.Wxbi',
                'Maria', 'Garcia', '(555) 302-0001', TRUE, TRUE, TRUE)
        RETURNING id INTO v_user_id;

        INSERT INTO user_roles (user_id, role) VALUES (v_user_id, 'student');
        INSERT INTO student_profile (user_id, preferred_pronouns, date_of_birth, gender,
                                     address_line1, city, state, zip_code,
                                     citizenship_status, curriculum_id, student_type,
                                     enrollment_date, expected_graduation, intake_status)
        VALUES (v_user_id, 'she/her', '1988-11-22', 'female',
                '456 Oak Avenue', 'Fort Worth', 'TX', '76102',
                'US Citizen', v_curriculum_class_a, 'full_time',
                '2025-12-02', '2025-12-30', 'completed');
        RAISE NOTICE 'Created student: maria.garcia@email.com (Class A)';
    END IF;

    -- Student 3: David Lee - Class A, pursuing HazMat endorsement
    IF NOT EXISTS (SELECT 1 FROM app_user WHERE email = 'david.lee@email.com') THEN
        INSERT INTO app_user (email, password_hash, first_name, last_name, phone_number, is_active, login_enabled, email_verified)
        VALUES ('david.lee@email.com', '$2a$12$LQv3c1yqBWVHxkd0LHAkCOYz6TtxMQJqhN8/X4bHCE5C1YXF.Wxbi',
                'David', 'Lee', '(555) 303-0001', TRUE, TRUE, TRUE)
        RETURNING id INTO v_user_id;

        INSERT INTO user_roles (user_id, role) VALUES (v_user_id, 'student');
        INSERT INTO student_profile (user_id, date_of_birth, gender,
                                     address_line1, city, state, zip_code,
                                     citizenship_status, curriculum_id, student_type,
                                     enrollment_date, expected_graduation, intake_status)
        VALUES (v_user_id, '1995-03-08', 'male',
                '789 Pine Road', 'Arlington', 'TX', '76010',
                'US Citizen', v_curriculum_class_a, 'full_time',
                '2025-11-04', '2025-12-02', 'completed');
        RAISE NOTICE 'Created student: david.lee@email.com (Class A)';
    END IF;

    -- Student 4: Lisa Chen - Class B, Passenger endorsement track
    IF NOT EXISTS (SELECT 1 FROM app_user WHERE email = 'lisa.chen@email.com') THEN
        INSERT INTO app_user (email, password_hash, first_name, last_name, phone_number, is_active, login_enabled, email_verified)
        VALUES ('lisa.chen@email.com', '$2a$12$LQv3c1yqBWVHxkd0LHAkCOYz6TtxMQJqhN8/X4bHCE5C1YXF.Wxbi',
                'Lisa', 'Chen', '(555) 304-0001', TRUE, TRUE, TRUE)
        RETURNING id INTO v_user_id;

        INSERT INTO user_roles (user_id, role) VALUES (v_user_id, 'student');
        INSERT INTO student_profile (user_id, preferred_pronouns, date_of_birth, gender,
                                     address_line1, city, state, zip_code,
                                     citizenship_status, curriculum_id, student_type,
                                     enrollment_date, expected_graduation, intake_status)
        VALUES (v_user_id, 'she/her', '1990-07-19', 'female',
                '321 Elm Street', 'Plano', 'TX', '75023',
                'US Citizen', v_curriculum_class_b, 'full_time',
                '2026-01-06', '2026-01-27', 'completed');
        RAISE NOTICE 'Created student: lisa.chen@email.com (Class B)';
    END IF;

    -- Student 5: Kevin Brown - Class B, School Bus track
    IF NOT EXISTS (SELECT 1 FROM app_user WHERE email = 'kevin.brown@email.com') THEN
        INSERT INTO app_user (email, password_hash, first_name, last_name, phone_number, is_active, login_enabled, email_verified)
        VALUES ('kevin.brown@email.com', '$2a$12$LQv3c1yqBWVHxkd0LHAkCOYz6TtxMQJqhN8/X4bHCE5C1YXF.Wxbi',
                'Kevin', 'Brown', '(555) 305-0001', TRUE, TRUE, TRUE)
        RETURNING id INTO v_user_id;

        INSERT INTO user_roles (user_id, role) VALUES (v_user_id, 'student');
        INSERT INTO student_profile (user_id, date_of_birth, gender,
                                     address_line1, city, state, zip_code,
                                     citizenship_status, curriculum_id, student_type,
                                     enrollment_date, expected_graduation, intake_status)
        VALUES (v_user_id, '1985-12-03', 'male',
                '555 Cedar Lane', 'Irving', 'TX', '75038',
                'US Citizen', v_curriculum_class_b, 'full_time',
                '2025-10-07', '2025-10-28', 'completed');
        RAISE NOTICE 'Created student: kevin.brown@email.com (Class B)';
    END IF;

    -- Student 6: Amanda Wilson - Class A, new student (intake in progress)
    IF NOT EXISTS (SELECT 1 FROM app_user WHERE email = 'amanda.wilson@email.com') THEN
        INSERT INTO app_user (email, password_hash, first_name, last_name, phone_number, is_active, login_enabled, email_verified)
        VALUES ('amanda.wilson@email.com', '$2a$12$LQv3c1yqBWVHxkd0LHAkCOYz6TtxMQJqhN8/X4bHCE5C1YXF.Wxbi',
                'Amanda', 'Wilson', '(555) 306-0001', TRUE, TRUE, TRUE)
        RETURNING id INTO v_user_id;

        INSERT INTO user_roles (user_id, role) VALUES (v_user_id, 'student');
        INSERT INTO student_profile (user_id, date_of_birth, gender,
                                     address_line1, city, state, zip_code,
                                     curriculum_id, student_type,
                                     enrollment_date, intake_status)
        VALUES (v_user_id, '1998-09-25', 'female',
                '777 Birch Drive', 'Garland', 'TX', '75040',
                v_curriculum_class_a, 'full_time',
                '2026-01-20', 'in_progress');
        RAISE NOTICE 'Created student: amanda.wilson@email.com (Class A - New)';
    END IF;

    -- Student 7: Marcus Taylor - Class A, requires financial aid
    IF NOT EXISTS (SELECT 1 FROM app_user WHERE email = 'marcus.taylor@email.com') THEN
        INSERT INTO app_user (email, password_hash, first_name, last_name, phone_number, is_active, login_enabled, email_verified)
        VALUES ('marcus.taylor@email.com', '$2a$12$LQv3c1yqBWVHxkd0LHAkCOYz6TtxMQJqhN8/X4bHCE5C1YXF.Wxbi',
                'Marcus', 'Taylor', '(555) 307-0001', TRUE, TRUE, TRUE)
        RETURNING id INTO v_user_id;

        INSERT INTO user_roles (user_id, role) VALUES (v_user_id, 'student');
        INSERT INTO student_profile (user_id, date_of_birth, gender,
                                     address_line1, city, state, zip_code,
                                     citizenship_status, curriculum_id, student_type,
                                     enrollment_date, expected_graduation,
                                     requires_financial_aid, intake_status)
        VALUES (v_user_id, '1993-02-14', 'male',
                '888 Maple Court', 'Mesquite', 'TX', '75149',
                'US Citizen', v_curriculum_class_a, 'full_time',
                '2026-01-06', '2026-02-03',
                TRUE, 'completed');
        RAISE NOTICE 'Created student: marcus.taylor@email.com (Class A - Financial Aid)';
    END IF;

    -- Student 8: Carlos Rodriguez - Class A, permanent resident
    IF NOT EXISTS (SELECT 1 FROM app_user WHERE email = 'carlos.rodriguez@email.com') THEN
        INSERT INTO app_user (email, password_hash, first_name, last_name, phone_number, is_active, login_enabled, email_verified)
        VALUES ('carlos.rodriguez@email.com', '$2a$12$LQv3c1yqBWVHxkd0LHAkCOYz6TtxMQJqhN8/X4bHCE5C1YXF.Wxbi',
                'Carlos', 'Rodriguez', '(555) 308-0001', TRUE, TRUE, TRUE)
        RETURNING id INTO v_user_id;

        INSERT INTO user_roles (user_id, role) VALUES (v_user_id, 'student');
        INSERT INTO student_profile (user_id, date_of_birth, gender,
                                     address_line1, city, state, zip_code,
                                     citizenship_status, citizenship_country, visa_type,
                                     curriculum_id, student_type,
                                     enrollment_date, expected_graduation,
                                     is_international, intake_status)
        VALUES (v_user_id, '1991-08-30', 'male',
                '999 Walnut Street', 'Richardson', 'TX', '75080',
                'Permanent Resident', 'Mexico', 'Green Card',
                v_curriculum_class_a, 'full_time',
                '2026-01-13', '2026-02-10',
                FALSE, 'completed');
        RAISE NOTICE 'Created student: carlos.rodriguez@email.com (Class A - Permanent Resident)';
    END IF;

    RAISE NOTICE '=== Student Users Created ===';
END $$;

-- =====================================================
-- VERIFICATION & SUMMARY
-- =====================================================

DO $$
DECLARE
    user_count INTEGER;
    admin_count INTEGER;
    instructor_count INTEGER;
    student_count INTEGER;
    legacy_instructor_count INTEGER;
    availability_count INTEGER;
    qualification_count INTEGER;
BEGIN
    SELECT COUNT(*) INTO user_count FROM app_user;
    SELECT COUNT(*) INTO admin_count FROM user_roles WHERE role = 'admin';
    SELECT COUNT(*) INTO instructor_count FROM user_roles WHERE role = 'instructor';
    SELECT COUNT(*) INTO student_count FROM user_roles WHERE role = 'student';
    SELECT COUNT(*) INTO legacy_instructor_count FROM instructor;
    SELECT COUNT(*) INTO availability_count FROM instructor_availability;
    SELECT COUNT(*) INTO qualification_count FROM instructor_qualification;

    RAISE NOTICE '';
    RAISE NOTICE '==============================================';
    RAISE NOTICE 'Seed Data Installation Complete!';
    RAISE NOTICE '==============================================';
    RAISE NOTICE '';
    RAISE NOTICE 'UNIFIED AUTH SYSTEM (app_user):';
    RAISE NOTICE '  Total Users: %', user_count;
    RAISE NOTICE '  - Admins: %', admin_count;
    RAISE NOTICE '  - Instructors: %', instructor_count;
    RAISE NOTICE '  - Students: %', student_count;
    RAISE NOTICE '';
    RAISE NOTICE 'INSTRUCTOR DATA:';
    RAISE NOTICE '  Legacy Instructor Records: %', legacy_instructor_count;
    RAISE NOTICE '  Availability Slots: %', availability_count;
    RAISE NOTICE '  Qualifications: %', qualification_count;
    RAISE NOTICE '';
    RAISE NOTICE 'TEST CREDENTIALS (password: Password123!):';
    RAISE NOTICE '';
    RAISE NOTICE '  ADMINS:';
    RAISE NOTICE '    director@cdlschool.edu   (Super Admin)';
    RAISE NOTICE '    manager@cdlschool.edu    (Manager)';
    RAISE NOTICE '    admissions@cdlschool.edu (Staff)';
    RAISE NOTICE '';
    RAISE NOTICE '  INSTRUCTORS:';
    RAISE NOTICE '    j.williams@cdlschool.edu (Class A, Senior + Examiner)';
    RAISE NOTICE '    m.johnson@cdlschool.edu  (Class A, Instructor)';
    RAISE NOTICE '    s.davis@cdlschool.edu    (Class B, Instructor)';
    RAISE NOTICE '    r.thompson@cdlschool.edu (Examiner Only)';
    RAISE NOTICE '';
    RAISE NOTICE '  STUDENTS:';
    RAISE NOTICE '    john.smith@email.com     (Class A, Veteran)';
    RAISE NOTICE '    maria.garcia@email.com   (Class A)';
    RAISE NOTICE '    david.lee@email.com      (Class A)';
    RAISE NOTICE '    lisa.chen@email.com      (Class B)';
    RAISE NOTICE '    kevin.brown@email.com    (Class B)';
    RAISE NOTICE '    amanda.wilson@email.com  (Class A, New)';
    RAISE NOTICE '    marcus.taylor@email.com  (Class A, Financial Aid)';
    RAISE NOTICE '    carlos.rodriguez@email.com (Class A, Perm Resident)';
    RAISE NOTICE '';
    RAISE NOTICE '==============================================';
END $$;

-- Migration: 015_unified_user_authentication.sql
-- Description: Consolidates authentication into a single user table with role-based access control
-- Date: 2026-01-23

-- =============================================================================
-- Create user_role enum type
-- =============================================================================

-- Drop existing table named user_role if it exists (conflicts with enum type name)
DROP TABLE IF EXISTS user_role CASCADE;

-- Drop existing type if it's not an enum (from failed previous runs)
DO $$
BEGIN
    -- Check if user_role exists but is NOT an enum type
    IF EXISTS (
        SELECT 1 FROM pg_type t
        JOIN pg_namespace n ON t.typnamespace = n.oid
        WHERE t.typname = 'user_role'
        AND n.nspname = 'public'
        AND t.typtype != 'e'  -- 'e' = enum
    ) THEN
        DROP TYPE user_role CASCADE;
        RAISE NOTICE 'Dropped non-enum user_role type';
    END IF;
END $$;

DO $$ BEGIN
    CREATE TYPE user_role AS ENUM ('student', 'instructor', 'admin');
EXCEPTION
    WHEN duplicate_object THEN null;
END $$;

-- =============================================================================
-- Create unified user table
-- =============================================================================

CREATE TABLE IF NOT EXISTS app_user (
    id SERIAL PRIMARY KEY,
    email VARCHAR(200) NOT NULL UNIQUE,
    password_hash VARCHAR(500) NOT NULL,

    -- Basic profile
    first_name VARCHAR(100),
    last_name VARCHAR(100),
    middle_name VARCHAR(100),
    preferred_name VARCHAR(100),
    phone_number VARCHAR(30),

    -- Account status
    is_active BOOLEAN DEFAULT TRUE,
    login_enabled BOOLEAN DEFAULT TRUE,
    email_verified BOOLEAN DEFAULT FALSE,
    email_verified_at TIMESTAMP,

    -- Authentication tracking
    last_login_at TIMESTAMP,
    failed_login_attempts INTEGER DEFAULT 0,
    locked_until TIMESTAMP,
    password_changed_at TIMESTAMP,

    -- Timestamps
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Create index for email lookups
CREATE INDEX IF NOT EXISTS idx_app_user_email ON app_user(email);

-- =============================================================================
-- Create user_roles junction table (many-to-many for admins with multiple roles)
-- =============================================================================

CREATE TABLE IF NOT EXISTS user_roles (
    id SERIAL PRIMARY KEY,
    user_id INTEGER NOT NULL REFERENCES app_user(id) ON DELETE CASCADE,
    role user_role NOT NULL,
    granted_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    granted_by INTEGER REFERENCES app_user(id),
    is_active BOOLEAN DEFAULT TRUE,

    -- Ensure unique role per user
    UNIQUE(user_id, role)
);

-- Create index for role lookups
CREATE INDEX IF NOT EXISTS idx_user_roles_user_id ON user_roles(user_id);
CREATE INDEX IF NOT EXISTS idx_user_roles_role ON user_roles(role);

-- =============================================================================
-- Create student_profile table (extended data for student role)
-- =============================================================================

CREATE TABLE IF NOT EXISTS student_profile (
    id SERIAL PRIMARY KEY,
    user_id INTEGER NOT NULL UNIQUE REFERENCES app_user(id) ON DELETE CASCADE,

    -- Personal information
    preferred_pronouns VARCHAR(50),
    date_of_birth DATE,
    gender VARCHAR(20),
    alternate_phone VARCHAR(30),

    -- Address
    address_line1 VARCHAR(200),
    address_line2 VARCHAR(200),
    city VARCHAR(100),
    state VARCHAR(100),
    zip_code VARCHAR(20),

    -- Sensitive information
    ssn VARCHAR(20),
    citizenship_status VARCHAR(50),
    citizenship_country VARCHAR(100),
    visa_type VARCHAR(50),

    -- Academic information
    curriculum_id INTEGER REFERENCES curriculum(id),
    student_type VARCHAR(50),
    enrollment_date DATE,
    expected_graduation DATE,

    -- Flags
    is_international BOOLEAN DEFAULT FALSE,
    is_transfer_student BOOLEAN DEFAULT FALSE,
    is_veteran BOOLEAN DEFAULT FALSE,
    requires_financial_aid BOOLEAN DEFAULT FALSE,

    -- Intake process
    intake_status VARCHAR(50) DEFAULT 'in_progress',
    completed_forms TEXT,

    -- Timestamps
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Create index for curriculum
CREATE INDEX IF NOT EXISTS idx_student_profile_curriculum ON student_profile(curriculum_id);

-- =============================================================================
-- Create instructor_profile table (extended data for instructor role)
-- =============================================================================

CREATE TABLE IF NOT EXISTS instructor_profile (
    id SERIAL PRIMARY KEY,
    user_id INTEGER NOT NULL UNIQUE REFERENCES app_user(id) ON DELETE CASCADE,

    -- Employment info
    employee_id VARCHAR(50) UNIQUE,
    department_id INTEGER REFERENCES department(id),
    hire_date DATE,

    -- Instructor type
    instructor_type VARCHAR(50) NOT NULL DEFAULT 'instructor',
    -- instructor, examiner, or both

    -- CDL-specific qualifications
    cdl_number VARCHAR(50),
    cdl_class VARCHAR(10),
    cdl_expiration DATE,

    -- Capabilities
    is_certified_examiner BOOLEAN DEFAULT FALSE,
    can_schedule BOOLEAN DEFAULT TRUE,
    can_validate BOOLEAN DEFAULT TRUE,
    can_issue_cdl BOOLEAN DEFAULT FALSE,

    -- Status
    is_active BOOLEAN DEFAULT TRUE,

    -- Timestamps
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,

    CONSTRAINT instructor_type_check
        CHECK (instructor_type IN ('instructor', 'examiner', 'both'))
);

-- Create index for department
CREATE INDEX IF NOT EXISTS idx_instructor_profile_department ON instructor_profile(department_id);

-- =============================================================================
-- Create admin_profile table (extended data for admin role)
-- =============================================================================

CREATE TABLE IF NOT EXISTS admin_profile (
    id SERIAL PRIMARY KEY,
    user_id INTEGER NOT NULL UNIQUE REFERENCES app_user(id) ON DELETE CASCADE,

    -- Employment info
    employee_id VARCHAR(50) UNIQUE,
    department_id INTEGER REFERENCES department(id),

    -- Admin level
    admin_level VARCHAR(50) NOT NULL DEFAULT 'staff',
    -- staff, manager, super_admin

    -- Permissions (can be further customized)
    can_manage_students BOOLEAN DEFAULT TRUE,
    can_manage_curriculum BOOLEAN DEFAULT FALSE,
    can_manage_instructors BOOLEAN DEFAULT FALSE,
    can_manage_admins BOOLEAN DEFAULT FALSE,
    can_view_reports BOOLEAN DEFAULT TRUE,
    can_revoke_access BOOLEAN DEFAULT FALSE,

    -- Status
    is_active BOOLEAN DEFAULT TRUE,

    -- Timestamps
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,

    CONSTRAINT admin_level_check
        CHECK (admin_level IN ('staff', 'manager', 'super_admin'))
);

-- =============================================================================
-- Create user_session table for token management
-- =============================================================================

CREATE TABLE IF NOT EXISTS user_session (
    id SERIAL PRIMARY KEY,
    user_id INTEGER NOT NULL REFERENCES app_user(id) ON DELETE CASCADE,
    session_token VARCHAR(500) NOT NULL UNIQUE,
    refresh_token VARCHAR(500),

    -- Session info
    ip_address VARCHAR(50),
    user_agent TEXT,
    device_info TEXT,

    -- Active role for this session (for multi-role users)
    active_role user_role,

    -- Timestamps
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    expires_at TIMESTAMP NOT NULL,
    last_activity_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,

    -- Status
    is_active BOOLEAN DEFAULT TRUE,
    revoked_at TIMESTAMP,
    revoked_reason VARCHAR(200)
);

-- Create indexes
CREATE INDEX IF NOT EXISTS idx_user_session_user_id ON user_session(user_id);
CREATE INDEX IF NOT EXISTS idx_user_session_token ON user_session(session_token);
CREATE INDEX IF NOT EXISTS idx_user_session_expires ON user_session(expires_at);

-- =============================================================================
-- Create login_audit table for security tracking
-- =============================================================================

CREATE TABLE IF NOT EXISTS login_audit (
    id SERIAL PRIMARY KEY,
    user_id INTEGER REFERENCES app_user(id),
    email VARCHAR(200) NOT NULL,

    -- Audit info
    action VARCHAR(50) NOT NULL,
    -- login_success, login_failed, logout, password_reset, etc.

    success BOOLEAN NOT NULL,
    failure_reason VARCHAR(200),

    -- Context
    ip_address VARCHAR(50),
    user_agent TEXT,

    -- Timestamp
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Create indexes
CREATE INDEX IF NOT EXISTS idx_login_audit_user_id ON login_audit(user_id);
CREATE INDEX IF NOT EXISTS idx_login_audit_email ON login_audit(email);
CREATE INDEX IF NOT EXISTS idx_login_audit_created ON login_audit(created_at);

-- =============================================================================
-- Helper function to check if user has a specific role
-- =============================================================================

CREATE OR REPLACE FUNCTION user_has_role(p_user_id INTEGER, p_role user_role)
RETURNS BOOLEAN AS $$
BEGIN
    RETURN EXISTS (
        SELECT 1 FROM user_roles
        WHERE user_id = p_user_id
        AND role = p_role
        AND is_active = TRUE
    );
END;
$$ LANGUAGE plpgsql;

-- =============================================================================
-- Helper function to get user's primary role (for routing)
-- Priority: admin > instructor > student
-- =============================================================================

CREATE OR REPLACE FUNCTION get_user_primary_role(p_user_id INTEGER)
RETURNS user_role AS $$
DECLARE
    v_role user_role;
BEGIN
    -- Check for admin first (highest priority)
    IF user_has_role(p_user_id, 'admin'::user_role) THEN
        RETURN 'admin'::user_role;
    END IF;

    -- Check for instructor
    IF user_has_role(p_user_id, 'instructor'::user_role) THEN
        RETURN 'instructor'::user_role;
    END IF;

    -- Default to student
    IF user_has_role(p_user_id, 'student'::user_role) THEN
        RETURN 'student'::user_role;
    END IF;

    -- No role found
    RETURN NULL;
END;
$$ LANGUAGE plpgsql;

-- =============================================================================
-- Helper function to get all roles for a user
-- =============================================================================

CREATE OR REPLACE FUNCTION get_user_roles(p_user_id INTEGER)
RETURNS TABLE(role user_role) AS $$
BEGIN
    RETURN QUERY
    SELECT ur.role FROM user_roles ur
    WHERE ur.user_id = p_user_id
    AND ur.is_active = TRUE
    ORDER BY
        CASE ur.role
            WHEN 'admin'::user_role THEN 1
            WHEN 'instructor'::user_role THEN 2
            WHEN 'student'::user_role THEN 3
        END;
END;
$$ LANGUAGE plpgsql;

-- =============================================================================
-- Trigger to update updated_at timestamp
-- =============================================================================

CREATE OR REPLACE FUNCTION update_timestamp()
RETURNS TRIGGER AS $$
BEGIN
    NEW.updated_at = CURRENT_TIMESTAMP;
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

-- Apply triggers
DROP TRIGGER IF EXISTS update_app_user_timestamp ON app_user;
CREATE TRIGGER update_app_user_timestamp
    BEFORE UPDATE ON app_user
    FOR EACH ROW EXECUTE FUNCTION update_timestamp();

DROP TRIGGER IF EXISTS update_student_profile_timestamp ON student_profile;
CREATE TRIGGER update_student_profile_timestamp
    BEFORE UPDATE ON student_profile
    FOR EACH ROW EXECUTE FUNCTION update_timestamp();

DROP TRIGGER IF EXISTS update_instructor_profile_timestamp ON instructor_profile;
CREATE TRIGGER update_instructor_profile_timestamp
    BEFORE UPDATE ON instructor_profile
    FOR EACH ROW EXECUTE FUNCTION update_timestamp();

DROP TRIGGER IF EXISTS update_admin_profile_timestamp ON admin_profile;
CREATE TRIGGER update_admin_profile_timestamp
    BEFORE UPDATE ON admin_profile
    FOR EACH ROW EXECUTE FUNCTION update_timestamp();

-- =============================================================================
-- View for easy user data access with roles
-- =============================================================================

CREATE OR REPLACE VIEW user_with_roles AS
SELECT
    u.id,
    u.email,
    u.first_name,
    u.last_name,
    u.is_active,
    u.login_enabled,
    u.last_login_at,
    get_user_primary_role(u.id) as primary_role,
    ARRAY(SELECT role FROM get_user_roles(u.id)) as all_roles,
    (SELECT COUNT(*) FROM user_roles WHERE user_id = u.id AND is_active = TRUE) as role_count
FROM app_user u;

-- =============================================================================
-- Create default super admin user (password: Admin123!)
-- =============================================================================

DO $$
DECLARE
    v_user_id INTEGER;
BEGIN
    -- Check if super admin already exists
    IF NOT EXISTS (SELECT 1 FROM app_user WHERE email = 'admin@system.local') THEN
        -- Insert user
        INSERT INTO app_user (email, password_hash, first_name, last_name, is_active, login_enabled, email_verified)
        VALUES ('admin@system.local',
                '$2a$12$LQv3c1yqBWVHxkd0LHAkCOYz6TtxMQJqhN8/X4bHCE5C1YXF.Wxbi', -- bcrypt hash for Admin123!
                'System', 'Administrator', TRUE, TRUE, TRUE)
        RETURNING id INTO v_user_id;

        -- Assign admin role
        INSERT INTO user_roles (user_id, role) VALUES (v_user_id, 'admin'::user_role);

        -- Create admin profile
        INSERT INTO admin_profile (user_id, admin_level, can_manage_students, can_manage_curriculum,
                                   can_manage_instructors, can_manage_admins, can_view_reports, can_revoke_access)
        VALUES (v_user_id, 'super_admin', TRUE, TRUE, TRUE, TRUE, TRUE, TRUE);

        RAISE NOTICE 'Created default super admin user: admin@system.local';
    END IF;
END $$;

-- =============================================================================
-- Migration helper: Migrate existing students to new system
-- This should be run carefully in production with proper data verification
-- =============================================================================

-- Note: This is commented out by default. Uncomment and run manually if needed.
/*
DO $$
DECLARE
    v_student RECORD;
    v_user_id INTEGER;
BEGIN
    FOR v_student IN SELECT * FROM student WHERE email IS NOT NULL LOOP
        -- Check if user already exists
        IF NOT EXISTS (SELECT 1 FROM app_user WHERE email = v_student.email) THEN
            -- Create app_user
            INSERT INTO app_user (email, password_hash, first_name, last_name, middle_name,
                                  preferred_name, phone_number, is_active, login_enabled, created_at)
            VALUES (v_student.email, v_student.password_hash, v_student.first_name, v_student.last_name,
                    v_student.middle_name, v_student.preferred_name, v_student.phone_number,
                    v_student.login_enabled, v_student.login_enabled, v_student.created_at)
            RETURNING id INTO v_user_id;

            -- Assign student role
            INSERT INTO user_roles (user_id, role) VALUES (v_user_id, 'student'::user_role);

            -- Create student profile
            INSERT INTO student_profile (user_id, preferred_pronouns, date_of_birth, gender,
                                         alternate_phone, address_line1, address_line2, city, state, zip_code,
                                         ssn, citizenship_status, citizenship_country, visa_type,
                                         curriculum_id, student_type, enrollment_date, expected_graduation,
                                         is_international, is_transfer_student, is_veteran, requires_financial_aid,
                                         intake_status, completed_forms, created_at)
            VALUES (v_user_id, v_student.preferred_pronouns, v_student.date_of_birth, v_student.gender,
                    v_student.alternate_phone, v_student.address_line1, v_student.address_line2,
                    v_student.city, v_student.state, v_student.zip_code, v_student.ssn,
                    v_student.citizenship_status, v_student.citizenship_country, v_student.visa_type,
                    v_student.curriculum_id, v_student.student_type, v_student.enrollment_date,
                    v_student.expected_graduation, v_student.is_international, v_student.is_transfer_student,
                    v_student.is_veteran, v_student.requires_financial_aid, v_student.intake_status,
                    v_student.completed_forms, v_student.created_at);
        END IF;
    END LOOP;

    RAISE NOTICE 'Migration of existing students completed';
END $$;
*/

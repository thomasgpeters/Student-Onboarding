-- =====================================================
-- Migration 014: Consolidate Users to Single AppUser Table
-- =====================================================
-- This migration creates a unified user authentication system where:
-- - All users (students and admins) have credentials in app_user table
-- - Roles are assigned via user_role table
-- - student and admin_user become profile tables that reference app_user
--
-- For new installations: These changes are already in schema.sql
-- For existing installations: Run this migration script
--
-- Usage:
--   psql -U postgres -d student_onboarding -f database/migrations/014_consolidate_users_to_app_user.sql
-- =====================================================

BEGIN;

-- =====================================================
-- 1. Create app_user table - Single source of truth for credentials
-- =====================================================

CREATE TABLE IF NOT EXISTS app_user (
    id SERIAL PRIMARY KEY,
    email VARCHAR(200) NOT NULL UNIQUE,
    password_hash VARCHAR(500) NOT NULL,
    first_name VARCHAR(100),
    last_name VARCHAR(100),
    is_active BOOLEAN DEFAULT TRUE,
    login_enabled BOOLEAN DEFAULT TRUE,
    email_verified BOOLEAN DEFAULT FALSE,
    last_login_at TIMESTAMP,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- =====================================================
-- 2. Create user_role table - Role assignments
-- =====================================================
-- A user can have multiple roles (e.g., both student and instructor)
-- Only administrators can assign roles

CREATE TABLE IF NOT EXISTS user_role (
    id SERIAL PRIMARY KEY,
    app_user_id INTEGER NOT NULL REFERENCES app_user(id) ON DELETE CASCADE,
    role VARCHAR(50) NOT NULL,
    assigned_by INTEGER REFERENCES app_user(id),
    assigned_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    is_active BOOLEAN DEFAULT TRUE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    -- Each user can only have each role once
    UNIQUE(app_user_id, role),
    -- Role constraint
    CONSTRAINT user_role_check
        CHECK (role IN ('student', 'instructor', 'admin', 'super_admin'))
);

-- =====================================================
-- 3. Add app_user_id to student table
-- =====================================================

DO $$
BEGIN
    IF NOT EXISTS (
        SELECT 1 FROM information_schema.columns
        WHERE table_name = 'student' AND column_name = 'app_user_id'
    ) THEN
        ALTER TABLE student ADD COLUMN app_user_id INTEGER REFERENCES app_user(id);
        RAISE NOTICE 'Added app_user_id column to student table';
    ELSE
        RAISE NOTICE 'app_user_id column already exists in student table';
    END IF;
END $$;

-- =====================================================
-- 4. Add app_user_id to admin_user table
-- =====================================================

DO $$
BEGIN
    IF NOT EXISTS (
        SELECT 1 FROM information_schema.columns
        WHERE table_name = 'admin_user' AND column_name = 'app_user_id'
    ) THEN
        ALTER TABLE admin_user ADD COLUMN app_user_id INTEGER REFERENCES app_user(id);
        RAISE NOTICE 'Added app_user_id column to admin_user table';
    ELSE
        RAISE NOTICE 'app_user_id column already exists in admin_user table';
    END IF;
END $$;

-- =====================================================
-- 5. Migrate existing student data to app_user
-- =====================================================

-- Create app_user entries for existing students with passwords
INSERT INTO app_user (email, password_hash, first_name, last_name, is_active, login_enabled)
SELECT
    s.email,
    COALESCE(s.password_hash, 'PENDING_PASSWORD_RESET'),
    s.first_name,
    s.last_name,
    CASE WHEN s.status = 'revoked' THEN FALSE ELSE TRUE END,
    COALESCE(s.login_enabled, TRUE)
FROM student s
WHERE NOT EXISTS (SELECT 1 FROM app_user au WHERE au.email = s.email)
ON CONFLICT (email) DO NOTHING;

-- Link students to their app_user records
UPDATE student s
SET app_user_id = au.id
FROM app_user au
WHERE s.email = au.email AND s.app_user_id IS NULL;

-- Create student roles for migrated students
INSERT INTO user_role (app_user_id, role, assigned_at)
SELECT au.id, 'student', s.created_at
FROM student s
JOIN app_user au ON s.app_user_id = au.id
WHERE NOT EXISTS (
    SELECT 1 FROM user_role ur
    WHERE ur.app_user_id = au.id AND ur.role = 'student'
);

-- =====================================================
-- 6. Migrate existing admin_user data to app_user
-- =====================================================

-- Create app_user entries for existing admins
INSERT INTO app_user (email, password_hash, first_name, last_name, is_active, login_enabled)
SELECT
    a.email,
    a.password_hash,
    a.first_name,
    a.last_name,
    a.is_active,
    TRUE
FROM admin_user a
WHERE NOT EXISTS (SELECT 1 FROM app_user au WHERE au.email = a.email)
ON CONFLICT (email) DO UPDATE SET
    -- If email already exists (e.g., someone who is both student and admin), update password if needed
    password_hash = CASE
        WHEN app_user.password_hash = 'PENDING_PASSWORD_RESET' THEN EXCLUDED.password_hash
        ELSE app_user.password_hash
    END;

-- Link admin_user records to their app_user records
UPDATE admin_user a
SET app_user_id = au.id
FROM app_user au
WHERE a.email = au.email AND a.app_user_id IS NULL;

-- Create admin roles for migrated admins
INSERT INTO user_role (app_user_id, role, assigned_at)
SELECT au.id, a.role, a.created_at
FROM admin_user a
JOIN app_user au ON a.app_user_id = au.id
WHERE NOT EXISTS (
    SELECT 1 FROM user_role ur
    WHERE ur.app_user_id = au.id AND ur.role = a.role
);

-- =====================================================
-- 7. Create indexes for performance
-- =====================================================

CREATE INDEX IF NOT EXISTS idx_app_user_email ON app_user(email);
CREATE INDEX IF NOT EXISTS idx_app_user_active ON app_user(is_active);
CREATE INDEX IF NOT EXISTS idx_user_role_user ON user_role(app_user_id);
CREATE INDEX IF NOT EXISTS idx_user_role_role ON user_role(role);
CREATE INDEX IF NOT EXISTS idx_student_app_user ON student(app_user_id);
CREATE INDEX IF NOT EXISTS idx_admin_user_app_user ON admin_user(app_user_id);

-- =====================================================
-- 8. Create triggers for updated_at
-- =====================================================

DROP TRIGGER IF EXISTS update_app_user_updated_at ON app_user;
CREATE TRIGGER update_app_user_updated_at
    BEFORE UPDATE ON app_user
    FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

DROP TRIGGER IF EXISTS update_user_role_updated_at ON user_role;
CREATE TRIGGER update_user_role_updated_at
    BEFORE UPDATE ON user_role
    FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

COMMIT;

-- =====================================================
-- Verification
-- =====================================================

DO $$
DECLARE
    app_user_exists BOOLEAN;
    user_role_exists BOOLEAN;
    student_col_exists BOOLEAN;
    admin_col_exists BOOLEAN;
    migrated_students INTEGER;
    migrated_admins INTEGER;
BEGIN
    -- Check tables exist
    SELECT EXISTS (
        SELECT 1 FROM information_schema.tables WHERE table_name = 'app_user'
    ) INTO app_user_exists;

    SELECT EXISTS (
        SELECT 1 FROM information_schema.tables WHERE table_name = 'user_role'
    ) INTO user_role_exists;

    -- Check columns exist
    SELECT EXISTS (
        SELECT 1 FROM information_schema.columns
        WHERE table_name = 'student' AND column_name = 'app_user_id'
    ) INTO student_col_exists;

    SELECT EXISTS (
        SELECT 1 FROM information_schema.columns
        WHERE table_name = 'admin_user' AND column_name = 'app_user_id'
    ) INTO admin_col_exists;

    -- Count migrated records
    SELECT COUNT(*) FROM student WHERE app_user_id IS NOT NULL INTO migrated_students;
    SELECT COUNT(*) FROM admin_user WHERE app_user_id IS NOT NULL INTO migrated_admins;

    IF app_user_exists AND user_role_exists AND student_col_exists AND admin_col_exists THEN
        RAISE NOTICE '';
        RAISE NOTICE '==========================================';
        RAISE NOTICE 'Migration 014 completed successfully!';
        RAISE NOTICE '==========================================';
        RAISE NOTICE 'Changes applied:';
        RAISE NOTICE '  - app_user table: OK';
        RAISE NOTICE '  - user_role table: OK';
        RAISE NOTICE '  - student.app_user_id column: OK';
        RAISE NOTICE '  - admin_user.app_user_id column: OK';
        RAISE NOTICE '';
        RAISE NOTICE 'Migration stats:';
        RAISE NOTICE '  - Students linked to app_user: %', migrated_students;
        RAISE NOTICE '  - Admins linked to app_user: %', migrated_admins;
        RAISE NOTICE '';
        RAISE NOTICE 'Note: email and password_hash columns remain in';
        RAISE NOTICE 'student and admin_user tables for backward compatibility.';
        RAISE NOTICE 'A future migration will remove these columns.';
        RAISE NOTICE '==========================================';
    ELSE
        RAISE WARNING 'Migration may have failed. Please check the database.';
    END IF;
END $$;

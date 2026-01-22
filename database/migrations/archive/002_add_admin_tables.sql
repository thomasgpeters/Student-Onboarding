-- Migration: Add Admin Portal Tables
-- Run this on existing databases to add admin functionality

-- =====================================================
-- ADMIN USER TABLE
-- =====================================================

CREATE TABLE IF NOT EXISTS admin_user (
    id SERIAL PRIMARY KEY,
    email VARCHAR(200) NOT NULL UNIQUE,
    password_hash VARCHAR(500) NOT NULL,
    first_name VARCHAR(100),
    last_name VARCHAR(100),
    role VARCHAR(50) NOT NULL DEFAULT 'instructor',  -- 'super_admin', 'administrator', 'instructor'
    department_id INTEGER REFERENCES department(id),
    is_active BOOLEAN DEFAULT TRUE,
    last_login TIMESTAMP,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Index for email lookup during login
CREATE INDEX IF NOT EXISTS idx_admin_user_email ON admin_user(email);

-- =====================================================
-- ADMIN PROGRAM ASSIGNMENT TABLE
-- Links instructors to specific programs they can view
-- =====================================================

CREATE TABLE IF NOT EXISTS admin_program_assignment (
    id SERIAL PRIMARY KEY,
    admin_user_id INTEGER NOT NULL REFERENCES admin_user(id) ON DELETE CASCADE,
    curriculum_id INTEGER NOT NULL REFERENCES curriculum(id) ON DELETE CASCADE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    UNIQUE(admin_user_id, curriculum_id)
);

CREATE INDEX IF NOT EXISTS idx_admin_program_admin ON admin_program_assignment(admin_user_id);
CREATE INDEX IF NOT EXISTS idx_admin_program_curriculum ON admin_program_assignment(curriculum_id);

-- =====================================================
-- ADMIN AUDIT LOG TABLE
-- Track all admin actions for security/compliance
-- =====================================================

CREATE TABLE IF NOT EXISTS admin_audit_log (
    id SERIAL PRIMARY KEY,
    admin_user_id INTEGER REFERENCES admin_user(id),
    action VARCHAR(100) NOT NULL,
    entity_type VARCHAR(100),  -- 'student', 'curriculum', 'form', etc.
    entity_id INTEGER,
    old_value TEXT,
    new_value TEXT,
    ip_address VARCHAR(50),
    user_agent VARCHAR(500),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX IF NOT EXISTS idx_admin_audit_user ON admin_audit_log(admin_user_id);
CREATE INDEX IF NOT EXISTS idx_admin_audit_action ON admin_audit_log(action);
CREATE INDEX IF NOT EXISTS idx_admin_audit_entity ON admin_audit_log(entity_type, entity_id);

-- =====================================================
-- STUDENT TABLE MODIFICATIONS
-- Add login revocation support
-- =====================================================

ALTER TABLE student ADD COLUMN IF NOT EXISTS is_login_revoked BOOLEAN DEFAULT FALSE;
ALTER TABLE student ADD COLUMN IF NOT EXISTS revoked_at TIMESTAMP;
ALTER TABLE student ADD COLUMN IF NOT EXISTS revoked_by INTEGER REFERENCES admin_user(id);
ALTER TABLE student ADD COLUMN IF NOT EXISTS revocation_reason TEXT;

-- =====================================================
-- CURRICULUM TABLE MODIFICATIONS
-- Add syllabus content storage
-- =====================================================

ALTER TABLE curriculum ADD COLUMN IF NOT EXISTS syllabus_content TEXT;
ALTER TABLE curriculum ADD COLUMN IF NOT EXISTS syllabus_updated_at TIMESTAMP;
ALTER TABLE curriculum ADD COLUMN IF NOT EXISTS syllabus_updated_by INTEGER REFERENCES admin_user(id);

-- =====================================================
-- CURRICULUM_FORM_REQUIREMENT TABLE MODIFICATIONS
-- Add ordering and tracking
-- =====================================================

ALTER TABLE curriculum_form_requirement ADD COLUMN IF NOT EXISTS display_order INTEGER DEFAULT 0;
ALTER TABLE curriculum_form_requirement ADD COLUMN IF NOT EXISTS is_required BOOLEAN DEFAULT TRUE;
ALTER TABLE curriculum_form_requirement ADD COLUMN IF NOT EXISTS added_by INTEGER REFERENCES admin_user(id);
ALTER TABLE curriculum_form_requirement ADD COLUMN IF NOT EXISTS added_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP;

-- =====================================================
-- FUNCTION: Update timestamp trigger (create if not exists)
-- =====================================================

CREATE OR REPLACE FUNCTION update_updated_at_column()
RETURNS TRIGGER AS $$
BEGIN
    NEW.updated_at = CURRENT_TIMESTAMP;
    RETURN NEW;
END;
$$ language 'plpgsql';

-- =====================================================
-- TRIGGER: Update timestamp for admin_user
-- =====================================================

DROP TRIGGER IF EXISTS update_admin_user_updated_at ON admin_user;
CREATE TRIGGER update_admin_user_updated_at
    BEFORE UPDATE ON admin_user
    FOR EACH ROW
    EXECUTE FUNCTION update_updated_at_column();

-- =====================================================
-- SEED DATA: Initial Super Admin Account
-- Password: 'admin123' (bcrypt hash) - CHANGE IN PRODUCTION!
-- =====================================================

INSERT INTO admin_user (email, password_hash, first_name, last_name, role, is_active)
VALUES (
    'admin@university.edu',
    '$2a$10$N9qo8uLOickgx2ZMRZoMyeIjZAgcfl7p92ldGxad68LJZdL17lhWy',  -- bcrypt hash for 'admin123' - CHANGE IN PRODUCTION!
    'System',
    'Administrator',
    'super_admin',
    TRUE
) ON CONFLICT (email) DO NOTHING;

-- =====================================================
-- COMMENTS
-- =====================================================

COMMENT ON TABLE admin_user IS 'Administrative users for the admin portal (staff, instructors, admins)';
COMMENT ON TABLE admin_program_assignment IS 'Links instructors to programs they can manage/view';
COMMENT ON TABLE admin_audit_log IS 'Audit trail of all administrative actions';
COMMENT ON COLUMN student.is_login_revoked IS 'Whether student login has been revoked by admin';
COMMENT ON COLUMN student.revoked_by IS 'Admin user who revoked student access';
COMMENT ON COLUMN curriculum.syllabus_content IS 'Rich text content for program syllabus';
COMMENT ON COLUMN curriculum_form_requirement.display_order IS 'Order in which forms appear for this program';

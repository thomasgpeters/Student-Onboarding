-- =============================================================================
-- Student Onboarding System - Installation Script
-- =============================================================================
--
-- USAGE - Command Line (psql):
--   createdb student_onboarding
--   psql -U postgres -d student_onboarding -f database/schema.sql
--   psql -U postgres -d student_onboarding -f database/install.sql
--   psql -U postgres -d student_onboarding -f database/scripts/switch_to_vocational.sql
--
-- USAGE - GUI Tools (pgAdmin, DBeaver, etc.):
--   1. Create database 'student_onboarding'
--   2. Run schema.sql first
--   3. Run this file (install.sql)
--   4. Run switch_to_accredited.sql OR switch_to_vocational.sql
--
-- =============================================================================

-- =====================================================
-- Step 1: Verify schema exists
-- =====================================================
DO $$
BEGIN
    IF NOT EXISTS (SELECT 1 FROM information_schema.tables WHERE table_name = 'form_type') THEN
        RAISE EXCEPTION 'Schema not found. Please run schema.sql first.';
    END IF;
    RAISE NOTICE 'Schema verified. Proceeding with installation...';
END $$;

-- =====================================================
-- Step 2: Seed Form Types
-- =====================================================

INSERT INTO form_type (code, name, description, category, display_order, is_required, required_for_international, required_for_transfer, required_for_veteran, required_for_financial_aid) VALUES
('personal_info', 'Personal Information', 'Basic personal details and contact information', 'core', 1, TRUE, FALSE, FALSE, FALSE, FALSE),
('emergency_contact', 'Emergency Contacts', 'Emergency contact information', 'core', 2, TRUE, FALSE, FALSE, FALSE, FALSE),
('medical_info', 'Medical Information', 'Health and medical information', 'health', 3, FALSE, FALSE, FALSE, FALSE, FALSE),
('academic_history', 'Academic History', 'Educational background and transcripts', 'academic', 4, TRUE, FALSE, TRUE, FALSE, FALSE),
('financial_aid', 'Financial Aid', 'Financial aid application information', 'financial', 5, FALSE, FALSE, FALSE, FALSE, TRUE),
('documents', 'Document Upload', 'Required document submissions', 'documents', 6, FALSE, TRUE, TRUE, FALSE, FALSE),
('consent', 'Terms and Consent', 'Agreements and electronic signature', 'legal', 7, TRUE, FALSE, FALSE, FALSE, FALSE)
ON CONFLICT (code) DO UPDATE SET
    name = EXCLUDED.name,
    description = EXCLUDED.description,
    category = EXCLUDED.category,
    display_order = EXCLUDED.display_order,
    is_required = EXCLUDED.is_required,
    updated_at = CURRENT_TIMESTAMP;

-- =====================================================
-- Step 3: Seed Institution Settings
-- =====================================================

-- Branding settings
INSERT INTO institution_settings (setting_key, setting_value, setting_type, category, display_name, description, is_required, display_order)
VALUES
    ('institution_name', 'Student Onboarding System', 'string', 'branding', 'Institution Name', 'The official name of the institution', TRUE, 1),
    ('institution_tagline', 'Welcome to your academic journey', 'string', 'branding', 'Tagline', 'A short tagline or motto', FALSE, 2),
    ('institution_logo_url', '/resources/images/logo.png', 'string', 'branding', 'Logo URL', 'URL to the institution logo', FALSE, 3),
    ('primary_color', '#1e3a5f', 'string', 'branding', 'Primary Color', 'Primary brand color (hex)', FALSE, 4),
    ('secondary_color', '#3b82f6', 'string', 'branding', 'Secondary Color', 'Secondary brand color (hex)', FALSE, 5)
ON CONFLICT (setting_key) DO UPDATE SET
    setting_value = EXCLUDED.setting_value,
    updated_at = CURRENT_TIMESTAMP;

-- Contact settings
INSERT INTO institution_settings (setting_key, setting_value, setting_type, category, display_name, description, is_required, display_order)
VALUES
    ('contact_email', 'admissions@institution.edu', 'string', 'contact', 'Contact Email', 'Primary contact email address', TRUE, 10),
    ('contact_phone', '(555) 123-4567', 'string', 'contact', 'Contact Phone', 'Primary contact phone number', TRUE, 11),
    ('support_email', 'support@institution.edu', 'string', 'contact', 'Support Email', 'Technical support email address', FALSE, 12)
ON CONFLICT (setting_key) DO UPDATE SET
    setting_value = EXCLUDED.setting_value,
    updated_at = CURRENT_TIMESTAMP;

-- Location settings
INSERT INTO institution_settings (setting_key, setting_value, setting_type, category, display_name, description, is_required, display_order)
VALUES
    ('address_line1', '123 University Ave', 'string', 'location', 'Address Line 1', 'Street address', FALSE, 20),
    ('address_line2', '', 'string', 'location', 'Address Line 2', 'Suite, building, etc.', FALSE, 21),
    ('city', 'College Town', 'string', 'location', 'City', 'City name', FALSE, 22),
    ('state', 'State', 'string', 'location', 'State', 'State or province', FALSE, 23),
    ('zip_code', '12345', 'string', 'location', 'ZIP Code', 'Postal code', FALSE, 24),
    ('country', 'United States', 'string', 'location', 'Country', 'Country name', FALSE, 25)
ON CONFLICT (setting_key) DO UPDATE SET
    setting_value = EXCLUDED.setting_value,
    updated_at = CURRENT_TIMESTAMP;

-- General settings
INSERT INTO institution_settings (setting_key, setting_value, setting_type, category, display_name, description, is_required, display_order)
VALUES
    ('academic_year', '2024-2025', 'string', 'general', 'Academic Year', 'Current academic year', FALSE, 30),
    ('timezone', 'America/New_York', 'string', 'general', 'Timezone', 'Institution timezone', FALSE, 31),
    ('date_format', 'MM/DD/YYYY', 'string', 'general', 'Date Format', 'Preferred date format', FALSE, 32)
ON CONFLICT (setting_key) DO UPDATE SET
    setting_value = EXCLUDED.setting_value,
    updated_at = CURRENT_TIMESTAMP;

-- Institution type settings (for program mode switching)
INSERT INTO institution_settings (setting_key, setting_value, setting_type, category, display_name, description, is_required, display_order)
VALUES
    ('institution_type', 'vocational', 'string', 'general', 'Institution Type', 'Type of institution: accredited or vocational', TRUE, 40),
    ('program_mode', 'vocational', 'string', 'general', 'Program Mode', 'Current program mode: accredited, vocational, or hybrid', TRUE, 41),
    ('degree_types_enabled', '["certificate"]', 'json', 'general', 'Enabled Degree Types', 'JSON array of enabled degree types', TRUE, 42)
ON CONFLICT (setting_key) DO UPDATE SET
    setting_value = EXCLUDED.setting_value,
    updated_at = CURRENT_TIMESTAMP;

-- =====================================================
-- Step 4: Create Default Admin User (Unified AppUser Model)
-- =====================================================

-- First, create the AppUser entry (single source of truth for credentials)
-- Default admin user (password: admin123 - should be changed immediately)
-- Password hash is bcrypt of 'admin123'
INSERT INTO app_user (email, password_hash, first_name, last_name, is_active, login_enabled, email_verified)
VALUES (
    'admin@institution.edu',
    '$2b$12$LQv3c1yqBWVHxkd0LHAkCOYz6TtxMQJqhN8/X4.VttYlE/b1Qm1Vu',
    'System',
    'Administrator',
    TRUE,
    TRUE,
    TRUE
)
ON CONFLICT (email) DO NOTHING;

-- Create the super_admin role for the default admin
INSERT INTO user_role (app_user_id, role, is_active)
SELECT id, 'super_admin', TRUE
FROM app_user WHERE email = 'admin@institution.edu'
ON CONFLICT (app_user_id, role) DO NOTHING;

-- Create the admin_user profile entry (legacy compatibility)
-- Link it to the app_user via app_user_id
INSERT INTO admin_user (app_user_id, email, password_hash, first_name, last_name, role, is_active)
SELECT
    au.id,
    au.email,
    au.password_hash,
    au.first_name,
    au.last_name,
    'super_admin',
    TRUE
FROM app_user au WHERE au.email = 'admin@institution.edu'
ON CONFLICT (email) DO UPDATE SET
    app_user_id = EXCLUDED.app_user_id;

-- =====================================================
-- Verification & Summary
-- =====================================================

DO $$
DECLARE
    table_count INTEGER;
    form_type_count INTEGER;
    settings_count INTEGER;
    app_user_count INTEGER;
    user_role_count INTEGER;
    admin_count INTEGER;
BEGIN
    SELECT COUNT(*) INTO table_count FROM information_schema.tables
    WHERE table_schema = 'public' AND table_type = 'BASE TABLE';

    SELECT COUNT(*) INTO form_type_count FROM form_type;
    SELECT COUNT(*) INTO settings_count FROM institution_settings;
    SELECT COUNT(*) INTO app_user_count FROM app_user;
    SELECT COUNT(*) INTO user_role_count FROM user_role;
    SELECT COUNT(*) INTO admin_count FROM admin_user;

    RAISE NOTICE '';
    RAISE NOTICE '==============================================';
    RAISE NOTICE 'Installation Complete!';
    RAISE NOTICE '==============================================';
    RAISE NOTICE 'Tables created: %', table_count;
    RAISE NOTICE 'Form types: %', form_type_count;
    RAISE NOTICE 'Institution settings: %', settings_count;
    RAISE NOTICE 'App users (credentials): %', app_user_count;
    RAISE NOTICE 'User roles: %', user_role_count;
    RAISE NOTICE 'Admin profiles: %', admin_count;
    RAISE NOTICE '';
    RAISE NOTICE 'User Management Architecture:';
    RAISE NOTICE '  - All user credentials stored in app_user table';
    RAISE NOTICE '  - Roles assigned via user_role table';
    RAISE NOTICE '  - Administrators create users and assign roles';
    RAISE NOTICE '';
    RAISE NOTICE 'Next steps:';
    RAISE NOTICE '  1. Run a curriculum mode script:';
    RAISE NOTICE '     - switch_to_accredited.sql (universities)';
    RAISE NOTICE '     - switch_to_vocational.sql (trade schools)';
    RAISE NOTICE '';
    RAISE NOTICE '  2. Change the default admin password!';
    RAISE NOTICE '     Login: admin@institution.edu';
    RAISE NOTICE '     Password: admin123';
    RAISE NOTICE '==============================================';
END $$;

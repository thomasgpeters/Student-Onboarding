-- =============================================================================
-- Student Onboarding System - Installation Script
-- =============================================================================
-- Single entry point for database installation
--
-- Usage:
--   # Create database first (if needed)
--   createdb student_onboarding
--
--   # Run installation
--   psql -U postgres -d student_onboarding -f database/install.sql
--
--   # Then choose a curriculum mode:
--   psql -U postgres -d student_onboarding -f database/scripts/switch_to_accredited.sql
--   # OR
--   psql -U postgres -d student_onboarding -f database/scripts/switch_to_vocational.sql
--
-- =============================================================================

\echo '=============================================='
\echo 'Student Onboarding System - Installation'
\echo '=============================================='

-- =====================================================
-- Step 1: Create Schema (tables, indexes, triggers)
-- =====================================================
\echo ''
\echo 'Step 1: Creating database schema...'
\i schema.sql

-- =====================================================
-- Step 2: Seed Form Types
-- =====================================================
\echo ''
\echo 'Step 2: Seeding form types...'

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
\echo ''
\echo 'Step 3: Seeding institution settings...'

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
-- Step 4: Create Default Admin User
-- =====================================================
\echo ''
\echo 'Step 4: Creating default admin user...'

-- Default admin user (password: admin123 - should be changed immediately)
-- Password hash is bcrypt of 'admin123'
INSERT INTO admin_user (email, password_hash, first_name, last_name, role, is_active)
VALUES (
    'admin@institution.edu',
    '$2b$12$LQv3c1yqBWVHxkd0LHAkCOYz6TtxMQJqhN8/X4.VttYlE/b1Qm1Vu',
    'System',
    'Administrator',
    'super_admin',
    TRUE
)
ON CONFLICT (email) DO NOTHING;

-- =====================================================
-- Installation Complete
-- =====================================================
\echo ''
\echo '=============================================='
\echo 'Installation Complete!'
\echo '=============================================='
\echo ''
\echo 'Next steps:'
\echo '  1. Choose a curriculum mode:'
\echo '     - For universities/colleges (accredited):'
\echo '       psql -d student_onboarding -f database/scripts/switch_to_accredited.sql'
\echo ''
\echo '     - For trade schools/CDL (vocational):'
\echo '       psql -d student_onboarding -f database/scripts/switch_to_vocational.sql'
\echo ''
\echo '  2. Change the default admin password!'
\echo '     Default login: admin@institution.edu'
\echo ''
\echo '  3. Update institution settings in the admin portal'
\echo ''
\echo '=============================================='

-- =====================================================
-- Verify Installation
-- =====================================================
\echo ''
\echo 'Verification:'

SELECT 'Tables created: ' || COUNT(*)::text FROM information_schema.tables
WHERE table_schema = 'public' AND table_type = 'BASE TABLE';

SELECT 'Form types: ' || COUNT(*)::text FROM form_type;

SELECT 'Institution settings: ' || COUNT(*)::text FROM institution_settings;

SELECT 'Admin users: ' || COUNT(*)::text FROM admin_user;

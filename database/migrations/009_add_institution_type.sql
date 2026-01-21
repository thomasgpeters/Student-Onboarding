-- Migration 009: Add institution_type setting
-- Allows tracking whether the system is configured for accredited or vocational programs

-- Add institution_type to settings (if not exists)
INSERT INTO institution_settings (setting_key, setting_value, setting_type, category, display_name, description, is_required, display_order)
VALUES (
    'institution_type',
    'vocational',  -- Default to vocational for CDL schools
    'string',
    'general',
    'Institution Type',
    'Type of institution: accredited (university/college with degree programs) or vocational (trade school, CDL training)',
    TRUE,
    0
)
ON CONFLICT (setting_key) DO NOTHING;

-- Add program_mode setting to track current mode
INSERT INTO institution_settings (setting_key, setting_value, setting_type, category, display_name, description, is_required, display_order)
VALUES (
    'program_mode',
    'vocational',  -- Options: accredited, vocational, hybrid
    'string',
    'general',
    'Program Mode',
    'Current program configuration: accredited (academic degrees), vocational (trade certificates), or hybrid (both)',
    TRUE,
    1
)
ON CONFLICT (setting_key) DO NOTHING;

-- Add degree_types_enabled - JSON array of enabled degree types
INSERT INTO institution_settings (setting_key, setting_value, setting_type, category, display_name, description, is_required, display_order)
VALUES (
    'degree_types_enabled',
    '["certificate"]',  -- Default to just certificates for vocational
    'json',
    'general',
    'Enabled Degree Types',
    'JSON array of enabled degree types for this institution',
    FALSE,
    2
)
ON CONFLICT (setting_key) DO NOTHING;

-- Log the migration
DO $$
BEGIN
    RAISE NOTICE 'Migration 009: Added institution_type, program_mode, and degree_types_enabled settings';
END $$;

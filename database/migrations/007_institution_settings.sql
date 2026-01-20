-- Institution Settings Table
-- Stores configurable settings for the institution/school

CREATE TABLE IF NOT EXISTS institution_settings (
    setting_key VARCHAR(100) PRIMARY KEY,
    setting_value TEXT,
    setting_type VARCHAR(50) DEFAULT 'string',  -- string, boolean, integer, json
    category VARCHAR(50) DEFAULT 'general',     -- general, branding, contact, location
    display_name VARCHAR(200),
    description TEXT,
    is_required BOOLEAN DEFAULT FALSE,
    display_order INTEGER DEFAULT 0,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Create trigger for updated_at
CREATE TRIGGER update_institution_settings_updated_at
    BEFORE UPDATE ON institution_settings
    FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

-- Seed default institution settings
INSERT INTO institution_settings (setting_key, setting_value, setting_type, category, display_name, description, is_required, display_order) VALUES
-- Branding settings
('institution_name', 'Student Intake System', 'string', 'branding', 'Institution Name', 'The official name of your institution', TRUE, 1),
('institution_tagline', 'Official Student Records', 'string', 'branding', 'Tagline', 'A short tagline or subtitle for your institution', FALSE, 2),
('institution_logo_url', '', 'string', 'branding', 'Logo URL', 'URL to your institution''s logo image', FALSE, 3),

-- Contact settings
('contact_email', '', 'string', 'contact', 'Contact Email', 'Main contact email for the institution', FALSE, 10),
('contact_phone', '', 'string', 'contact', 'Contact Phone', 'Main phone number', FALSE, 11),
('contact_fax', '', 'string', 'contact', 'Fax Number', 'Fax number (if applicable)', FALSE, 12),
('website_url', '', 'string', 'contact', 'Website URL', 'Institution website address', FALSE, 13),

-- Primary location/address
('address_street1', '', 'string', 'location', 'Street Address', 'Primary street address line 1', FALSE, 20),
('address_street2', '', 'string', 'location', 'Street Address 2', 'Primary street address line 2', FALSE, 21),
('address_city', '', 'string', 'location', 'City', 'City', FALSE, 22),
('address_state', '', 'string', 'location', 'State/Province', 'State or province', FALSE, 23),
('address_postal_code', '', 'string', 'location', 'Postal Code', 'ZIP or postal code', FALSE, 24),
('address_country', 'United States', 'string', 'location', 'Country', 'Country', FALSE, 25),

-- Additional settings
('academic_year', '', 'string', 'general', 'Current Academic Year', 'Current academic year (e.g., 2024-2025)', FALSE, 30),
('accreditation_info', '', 'string', 'general', 'Accreditation', 'Accreditation information', FALSE, 31),
('timezone', 'America/New_York', 'string', 'general', 'Timezone', 'Institution timezone', FALSE, 32);

-- Index for faster lookups by category
CREATE INDEX idx_institution_settings_category ON institution_settings(category);

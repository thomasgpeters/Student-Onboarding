-- Migration: Add missing fields to Student table
-- Run this on existing databases to add the new columns

-- =====================================================
-- PostgreSQL version
-- =====================================================

-- Add new columns to student table (if not exists)
ALTER TABLE student ADD COLUMN IF NOT EXISTS preferred_pronouns VARCHAR(50);
ALTER TABLE student ADD COLUMN IF NOT EXISTS ssn VARCHAR(20);
ALTER TABLE student ADD COLUMN IF NOT EXISTS citizenship_status VARCHAR(50);
ALTER TABLE student ADD COLUMN IF NOT EXISTS completed_forms TEXT;
ALTER TABLE student ADD COLUMN IF NOT EXISTS phone_number VARCHAR(30);

-- Note: Address fields are stored in the student_address table, not student table
-- The student_address table supports multiple addresses per student:
--   address_type: 'permanent', 'mailing', 'billing'
--   is_primary: true/false

-- If you had address columns on student table, you can migrate them:
-- INSERT INTO student_address (student_id, address_type, street1, street2, city, state, postal_code, is_primary)
-- SELECT id, 'permanent', address_line1, address_line2, city, state, zip_code, true
-- FROM student WHERE address_line1 IS NOT NULL;

-- Then optionally drop the old columns:
-- ALTER TABLE student DROP COLUMN IF EXISTS address_line1;
-- ALTER TABLE student DROP COLUMN IF EXISTS address_line2;
-- ALTER TABLE student DROP COLUMN IF EXISTS city;
-- ALTER TABLE student DROP COLUMN IF EXISTS state;
-- ALTER TABLE student DROP COLUMN IF EXISTS zip_code;

-- =====================================================
-- SQLite version (run separately - no IF NOT EXISTS for columns)
-- =====================================================
-- ALTER TABLE Student ADD COLUMN PreferredPronouns VARCHAR(50);
-- ALTER TABLE Student ADD COLUMN Ssn VARCHAR(20);
-- ALTER TABLE Student ADD COLUMN CitizenshipStatus VARCHAR(50);
-- ALTER TABLE Student ADD COLUMN CompletedForms TEXT;
-- ALTER TABLE Student ADD COLUMN PhoneNumber VARCHAR(30);

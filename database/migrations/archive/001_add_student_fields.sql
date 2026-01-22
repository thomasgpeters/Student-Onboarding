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

-- Create student_address table if it doesn't exist
CREATE TABLE IF NOT EXISTS student_address (
    id SERIAL PRIMARY KEY,
    student_id INTEGER NOT NULL REFERENCES student(id),
    address_type VARCHAR(50) NOT NULL,  -- 'permanent', 'mailing', 'billing'
    street1 VARCHAR(200),
    street2 VARCHAR(200),
    city VARCHAR(100),
    state VARCHAR(100),
    postal_code VARCHAR(20),
    country VARCHAR(100),
    is_primary BOOLEAN DEFAULT FALSE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Create index for student_address if not exists
CREATE INDEX IF NOT EXISTS idx_student_address_student ON student_address(student_id);

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

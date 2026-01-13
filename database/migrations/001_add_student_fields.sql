-- Migration: Add missing fields to Student table
-- Run this on existing databases to add the new columns

-- PostgreSQL version
ALTER TABLE student ADD COLUMN IF NOT EXISTS preferred_pronouns VARCHAR(50);
ALTER TABLE student ADD COLUMN IF NOT EXISTS address_line1 VARCHAR(200);
ALTER TABLE student ADD COLUMN IF NOT EXISTS address_line2 VARCHAR(200);
ALTER TABLE student ADD COLUMN IF NOT EXISTS city VARCHAR(100);
ALTER TABLE student ADD COLUMN IF NOT EXISTS state VARCHAR(100);
ALTER TABLE student ADD COLUMN IF NOT EXISTS zip_code VARCHAR(20);
ALTER TABLE student ADD COLUMN IF NOT EXISTS ssn VARCHAR(20);
ALTER TABLE student ADD COLUMN IF NOT EXISTS citizenship_status VARCHAR(50);
ALTER TABLE student ADD COLUMN IF NOT EXISTS completed_forms TEXT;

-- Rename phone to phone_number if it exists (optional - handle gracefully)
-- Note: This may fail if the column doesn't exist or is already named correctly
-- ALTER TABLE student RENAME COLUMN phone TO phone_number;

-- SQLite version (doesn't support IF NOT EXISTS for columns, run separately)
-- ALTER TABLE Student ADD COLUMN PreferredPronouns VARCHAR(50);
-- ALTER TABLE Student ADD COLUMN AddressLine1 VARCHAR(200);
-- ALTER TABLE Student ADD COLUMN AddressLine2 VARCHAR(200);
-- ALTER TABLE Student ADD COLUMN City VARCHAR(100);
-- ALTER TABLE Student ADD COLUMN State VARCHAR(100);
-- ALTER TABLE Student ADD COLUMN ZipCode VARCHAR(20);
-- ALTER TABLE Student ADD COLUMN Ssn VARCHAR(20);
-- ALTER TABLE Student ADD COLUMN CitizenshipStatus VARCHAR(50);
-- ALTER TABLE Student ADD COLUMN CompletedForms TEXT;

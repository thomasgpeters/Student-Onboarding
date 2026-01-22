-- Migration: Change emergency_contact table to use compound primary key
-- Compound key: (student_id, contact_relationship, phone)
-- This prevents duplicate emergency contacts and allows proper PATCH updates

-- Step 1: Create new table with compound primary key
CREATE TABLE IF NOT EXISTS emergency_contact_new (
    student_id INTEGER NOT NULL REFERENCES student(id) ON DELETE CASCADE,
    first_name VARCHAR(100),
    last_name VARCHAR(100),
    contact_relationship VARCHAR(50) NOT NULL,
    phone VARCHAR(20) NOT NULL,
    alternate_phone VARCHAR(20),
    email VARCHAR(255),
    street1 VARCHAR(255),
    street2 VARCHAR(255),
    city VARCHAR(100),
    state VARCHAR(50),
    postal_code VARCHAR(20),
    country VARCHAR(100) DEFAULT 'United States',
    is_primary BOOLEAN DEFAULT FALSE,
    priority INTEGER DEFAULT 1,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (student_id, contact_relationship, phone)
);

-- Step 2: Migrate existing data (if table exists)
-- Use DISTINCT ON to handle any duplicates, keeping the most recent
INSERT INTO emergency_contact_new (
    student_id, first_name, last_name, contact_relationship, phone,
    alternate_phone, email, street1, street2, city, state, postal_code,
    country, is_primary, priority, created_at, updated_at
)
SELECT DISTINCT ON (student_id, contact_relationship, phone)
    student_id, first_name, last_name, contact_relationship, phone,
    alternate_phone, email, street1, street2, city, state, postal_code,
    country, is_primary, priority, created_at, updated_at
FROM emergency_contact
WHERE student_id IS NOT NULL
  AND contact_relationship IS NOT NULL
  AND phone IS NOT NULL
ORDER BY student_id, contact_relationship, phone, updated_at DESC;

-- Step 3: Drop old table and rename new one
DROP TABLE IF EXISTS emergency_contact CASCADE;
ALTER TABLE emergency_contact_new RENAME TO emergency_contact;

-- Step 4: Create indexes for common queries
CREATE INDEX IF NOT EXISTS idx_emergency_contact_student_id ON emergency_contact(student_id);
CREATE INDEX IF NOT EXISTS idx_emergency_contact_is_primary ON emergency_contact(student_id, is_primary);

-- Note: After running this migration, rebuild the ApiLogicServer to regenerate endpoints
-- The API will now use compound key format: /EmergencyContact/{student_id},{contact_relationship},{phone}

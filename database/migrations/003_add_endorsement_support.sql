-- =====================================================
-- Migration 003: Add Multi-Program Endorsement Support
-- =====================================================
-- This migration adds support for vocational/CDL programs where
-- students can enroll in a base program plus multiple endorsements.
--
-- For new installations: These changes are already in schema.sql
-- For existing installations: Run this migration script
--
-- Usage:
--   psql -U postgres -d student_onboarding -f database/migrations/003_add_endorsement_support.sql
-- =====================================================

BEGIN;

-- =====================================================
-- 1. Add is_endorsement column to curriculum table
-- =====================================================
-- This distinguishes base programs (FALSE) from add-on endorsements (TRUE)

DO $$
BEGIN
    IF NOT EXISTS (
        SELECT 1 FROM information_schema.columns
        WHERE table_name = 'curriculum' AND column_name = 'is_endorsement'
    ) THEN
        ALTER TABLE curriculum ADD COLUMN is_endorsement BOOLEAN DEFAULT FALSE;
        RAISE NOTICE 'Added is_endorsement column to curriculum table';
    ELSE
        RAISE NOTICE 'is_endorsement column already exists in curriculum table';
    END IF;
END $$;

-- =====================================================
-- 2. Create student_endorsement junction table
-- =====================================================
-- Tracks which endorsements each student is enrolled in
-- The student's base program is stored in student.curriculum_id

CREATE TABLE IF NOT EXISTS student_endorsement (
    id SERIAL PRIMARY KEY,
    student_id INTEGER NOT NULL REFERENCES student(id) ON DELETE CASCADE,
    curriculum_id INTEGER NOT NULL REFERENCES curriculum(id) ON DELETE CASCADE,
    enrollment_status VARCHAR(50) DEFAULT 'enrolled',
    enrolled_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    completed_at TIMESTAMP,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    -- Each student can only enroll in each endorsement once
    UNIQUE(student_id, curriculum_id),
    -- Enrollment status constraint
    CONSTRAINT student_endorsement_status_check
        CHECK (enrollment_status IN ('enrolled', 'in_progress', 'completed', 'withdrawn'))
);

-- =====================================================
-- 3. Create indexes for performance
-- =====================================================

CREATE INDEX IF NOT EXISTS idx_student_endorsement_student ON student_endorsement(student_id);
CREATE INDEX IF NOT EXISTS idx_student_endorsement_curriculum ON student_endorsement(curriculum_id);
CREATE INDEX IF NOT EXISTS idx_curriculum_is_endorsement ON curriculum(is_endorsement);

-- =====================================================
-- 4. Create updated_at trigger
-- =====================================================

-- Ensure the update_updated_at_column function exists
CREATE OR REPLACE FUNCTION update_updated_at_column()
RETURNS TRIGGER AS $$
BEGIN
    NEW.updated_at = CURRENT_TIMESTAMP;
    RETURN NEW;
END;
$$ language 'plpgsql';

-- Create trigger for student_endorsement table
DROP TRIGGER IF EXISTS update_student_endorsement_updated_at ON student_endorsement;
CREATE TRIGGER update_student_endorsement_updated_at
    BEFORE UPDATE ON student_endorsement
    FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

COMMIT;

-- =====================================================
-- Verification
-- =====================================================

DO $$
DECLARE
    col_exists BOOLEAN;
    tbl_exists BOOLEAN;
BEGIN
    -- Check is_endorsement column
    SELECT EXISTS (
        SELECT 1 FROM information_schema.columns
        WHERE table_name = 'curriculum' AND column_name = 'is_endorsement'
    ) INTO col_exists;

    -- Check student_endorsement table
    SELECT EXISTS (
        SELECT 1 FROM information_schema.tables
        WHERE table_name = 'student_endorsement'
    ) INTO tbl_exists;

    IF col_exists AND tbl_exists THEN
        RAISE NOTICE '';
        RAISE NOTICE '==========================================';
        RAISE NOTICE 'Migration 003 completed successfully!';
        RAISE NOTICE '==========================================';
        RAISE NOTICE 'Changes applied:';
        RAISE NOTICE '  - curriculum.is_endorsement column: OK';
        RAISE NOTICE '  - student_endorsement table: OK';
        RAISE NOTICE '';
        RAISE NOTICE 'Next steps:';
        RAISE NOTICE '  If using vocational mode, run:';
        RAISE NOTICE '  psql -d student_onboarding -f database/scripts/switch_to_vocational.sql';
        RAISE NOTICE '==========================================';
    ELSE
        RAISE WARNING 'Migration may have failed. Please check the database.';
    END IF;
END $$;

-- Migration 006: Flush Curriculum Data
-- This script clears all curriculum-related data for a fresh start
-- WARNING: This will delete all curriculum data including student references
--
-- Usage:
--   To flush and re-seed accredited only:
--     psql -f 006_flush_curriculum.sql
--     psql -f 005_seed_accredited_curriculum.sql
--
--   To flush and re-seed vocational only:
--     psql -f 006_flush_curriculum.sql
--     psql -f 005_seed_vocational_curriculum.sql
--
--   To flush and re-seed both:
--     psql -f 006_flush_curriculum.sql
--     psql -f 005_seed_accredited_curriculum.sql
--     psql -f 005_seed_vocational_curriculum.sql

-- =====================================================
-- IMPORTANT: Order matters due to foreign key constraints
-- =====================================================

-- Step 1: Clear curriculum form requirements (references curriculum and form_type)
DELETE FROM curriculum_form_requirement;
SELECT 'Cleared curriculum_form_requirement' AS status;

-- Step 2: Clear curriculum prerequisites (references curriculum)
DELETE FROM curriculum_prerequisite;
SELECT 'Cleared curriculum_prerequisite' AS status;

-- Step 3: Nullify student curriculum references (so we can delete curriculum)
UPDATE student SET curriculum_id = NULL WHERE curriculum_id IS NOT NULL;
SELECT 'Nullified student curriculum references' AS status;

-- Step 4: Clear curriculum table
DELETE FROM curriculum;
SELECT 'Cleared curriculum' AS status;

-- Step 5: Clear department table
DELETE FROM department;
SELECT 'Cleared department' AS status;

-- Step 6: Reset sequences for clean IDs
ALTER SEQUENCE IF EXISTS curriculum_id_seq RESTART WITH 1;
ALTER SEQUENCE IF EXISTS department_id_seq RESTART WITH 1;
ALTER SEQUENCE IF EXISTS curriculum_form_requirement_id_seq RESTART WITH 1;
ALTER SEQUENCE IF EXISTS curriculum_prerequisite_id_seq RESTART WITH 1;
SELECT 'Reset sequences' AS status;

-- Log completion
DO $$
BEGIN
    RAISE NOTICE '======================================';
    RAISE NOTICE 'Curriculum flush completed successfully';
    RAISE NOTICE 'Ready for re-seeding with:';
    RAISE NOTICE '  - 005_seed_accredited_curriculum.sql (accredited/academic programs)';
    RAISE NOTICE '  - 005_seed_vocational_curriculum.sql (vocational/CDL programs)';
    RAISE NOTICE '======================================';
END $$;

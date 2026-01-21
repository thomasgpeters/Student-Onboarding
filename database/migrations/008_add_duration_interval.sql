-- Migration 008: Add duration_interval field to curriculum table
-- Allows specifying the interval type: semester, month, week, day

-- Add the duration_interval column with default 'semester'
ALTER TABLE curriculum ADD COLUMN IF NOT EXISTS duration_interval VARCHAR(20) DEFAULT 'semester';

-- Add constraint to ensure valid interval values
DO $$
BEGIN
    IF NOT EXISTS (
        SELECT 1 FROM pg_constraint WHERE conname = 'curriculum_duration_interval_check'
    ) THEN
        ALTER TABLE curriculum ADD CONSTRAINT curriculum_duration_interval_check
            CHECK (duration_interval IN ('semester', 'month', 'week', 'day'));
    END IF;
END $$;

-- Update existing records to have 'semester' as default if null
UPDATE curriculum SET duration_interval = 'semester' WHERE duration_interval IS NULL;

-- Log the migration
DO $$
BEGIN
    RAISE NOTICE 'Migration 008: Added duration_interval column to curriculum table';
END $$;

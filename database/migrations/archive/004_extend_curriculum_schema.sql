-- Migration 004: Extend Curriculum Schema for Vocational Programs
-- Adds fields to support both accredited academic programs and non-accredited vocational programs (CDL training)

-- Add new columns to curriculum table
ALTER TABLE curriculum ADD COLUMN IF NOT EXISTS is_accredited BOOLEAN DEFAULT TRUE;
ALTER TABLE curriculum ADD COLUMN IF NOT EXISTS program_type VARCHAR(50) DEFAULT 'academic';
ALTER TABLE curriculum ADD COLUMN IF NOT EXISTS cdl_class VARCHAR(10);
ALTER TABLE curriculum ADD COLUMN IF NOT EXISTS endorsements TEXT;
ALTER TABLE curriculum ADD COLUMN IF NOT EXISTS training_hours_classroom INTEGER;
ALTER TABLE curriculum ADD COLUMN IF NOT EXISTS training_hours_range INTEGER;
ALTER TABLE curriculum ADD COLUMN IF NOT EXISTS training_hours_road INTEGER;

-- Add comment for documentation
COMMENT ON COLUMN curriculum.is_accredited IS 'Whether the program is accredited (TRUE for academic, FALSE for vocational/CDL)';
COMMENT ON COLUMN curriculum.program_type IS 'Program type: academic or vocational';
COMMENT ON COLUMN curriculum.cdl_class IS 'CDL class for vocational programs: A or B';
COMMENT ON COLUMN curriculum.endorsements IS 'JSON array of CDL endorsements: T, N, H, P, S, air_brakes';
COMMENT ON COLUMN curriculum.training_hours_classroom IS 'Classroom training hours for vocational programs';
COMMENT ON COLUMN curriculum.training_hours_range IS 'Range/yard training hours for vocational programs';
COMMENT ON COLUMN curriculum.training_hours_road IS 'On-road training hours for vocational programs';

-- Update existing curricula to be academic/accredited
UPDATE curriculum SET is_accredited = TRUE, program_type = 'academic' WHERE is_accredited IS NULL;

-- Create index for filtering by program type
CREATE INDEX IF NOT EXISTS idx_curriculum_program_type ON curriculum(program_type);
CREATE INDEX IF NOT EXISTS idx_curriculum_is_accredited ON curriculum(is_accredited);

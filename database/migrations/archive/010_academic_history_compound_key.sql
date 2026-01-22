-- Migration: Change academic_history to use compound primary key
-- Date: 2024
-- Description: Replace serial id with compound primary key (student_id, institution_name, institution_type)
--              This ensures uniqueness per student/institution/type combination

-- Drop the existing table and recreate with compound primary key
-- Note: This will lose existing data - run only on fresh installs or backup first

DROP TABLE IF EXISTS academic_history CASCADE;

CREATE TABLE academic_history (
    student_id INTEGER NOT NULL REFERENCES student(id) ON DELETE CASCADE,
    institution_name VARCHAR(200) NOT NULL,
    institution_type VARCHAR(50) NOT NULL,
    institution_city VARCHAR(100),
    institution_state VARCHAR(100),
    institution_country VARCHAR(100),
    degree_earned VARCHAR(100),
    major VARCHAR(200),
    minor VARCHAR(200),
    gpa DOUBLE PRECISION,
    gpa_scale DOUBLE PRECISION DEFAULT 4.0,
    start_date DATE,
    end_date DATE,
    graduation_date DATE,
    is_currently_attending BOOLEAN DEFAULT FALSE,
    transcript_received BOOLEAN DEFAULT FALSE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,

    -- Compound primary key
    PRIMARY KEY (student_id, institution_name, institution_type)
);

-- Create indexes for common queries
CREATE INDEX idx_academic_history_student ON academic_history(student_id);
CREATE INDEX idx_academic_history_type ON academic_history(institution_type);

-- Add comment explaining the key structure
-- Valid institution_type values: highschool, undergraduate, graduate, vocational
-- (avoid underscores to prevent compound key parsing issues)
COMMENT ON TABLE academic_history IS 'Student academic history with compound key (student_id, institution_name, institution_type)';

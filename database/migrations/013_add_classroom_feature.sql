-- =============================================================================
-- Migration 013: Add Classroom Feature
-- =============================================================================
-- Adds support for online courses with modules, content, assessments, and
-- time tracking for programs like the Entry Level Driver (ELD) prep-course.
--
-- Features:
-- - Online courses linked to curriculum
-- - Course modules (~32 for ELD)
-- - Module content (reading, video, quiz)
-- - Student course enrollment
-- - Student module progress tracking
-- - Time logging for regulatory compliance
-- - Assessments (quizzes and final exams)
-- - Assessment questions and answers
-- - Final assessment reports
-- =============================================================================

-- =====================================================
-- COURSE TABLE
-- =====================================================
-- Represents an online course that can be associated with a curriculum
-- E.g., ELD Prep Course for non-accredited CDL program

CREATE TABLE IF NOT EXISTS course (
    id SERIAL PRIMARY KEY,
    curriculum_id INTEGER REFERENCES curriculum(id) ON DELETE CASCADE,
    code VARCHAR(50) NOT NULL UNIQUE,
    name VARCHAR(200) NOT NULL,
    description TEXT,
    -- Course configuration
    total_modules INTEGER DEFAULT 0,
    estimated_hours INTEGER DEFAULT 0,
    passing_score INTEGER DEFAULT 70,
    -- Requirements
    requires_sequential_completion BOOLEAN DEFAULT TRUE,
    requires_time_tracking BOOLEAN DEFAULT TRUE,
    minimum_time_per_module INTEGER DEFAULT 0,  -- Minutes
    -- Status
    is_active BOOLEAN DEFAULT TRUE,
    published_at TIMESTAMP,
    -- Metadata
    version VARCHAR(20) DEFAULT '1.0',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- =====================================================
-- COURSE MODULE TABLE
-- =====================================================
-- Individual modules within a course (~32 for ELD)

CREATE TABLE IF NOT EXISTS course_module (
    id SERIAL PRIMARY KEY,
    course_id INTEGER NOT NULL REFERENCES course(id) ON DELETE CASCADE,
    module_number INTEGER NOT NULL,
    code VARCHAR(50) NOT NULL,
    title VARCHAR(200) NOT NULL,
    description TEXT,
    -- Module configuration
    estimated_minutes INTEGER DEFAULT 0,
    minimum_time_required INTEGER DEFAULT 0,  -- Minutes required before completion
    passing_score INTEGER DEFAULT 70,
    -- Prerequisites
    prerequisite_module_id INTEGER REFERENCES course_module(id),
    -- Status
    is_active BOOLEAN DEFAULT TRUE,
    -- Metadata
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    -- Unique constraint for module ordering within a course
    UNIQUE(course_id, module_number)
);

-- =====================================================
-- MODULE CONTENT TABLE
-- =====================================================
-- Content items within a module (reading, video, quiz)

CREATE TABLE IF NOT EXISTS module_content (
    id SERIAL PRIMARY KEY,
    module_id INTEGER NOT NULL REFERENCES course_module(id) ON DELETE CASCADE,
    content_order INTEGER NOT NULL,
    content_type VARCHAR(50) NOT NULL,  -- 'reading', 'video', 'quiz', 'interactive'
    title VARCHAR(200) NOT NULL,
    description TEXT,
    -- Content data
    content_url VARCHAR(1000),          -- For video or external resources
    content_text TEXT,                   -- For reading content (HTML/Markdown)
    content_duration INTEGER DEFAULT 0,  -- Duration in seconds (for video)
    -- Requirements
    is_required BOOLEAN DEFAULT TRUE,
    minimum_view_time INTEGER DEFAULT 0, -- Seconds required before marking complete
    -- Quiz reference (if content_type is 'quiz')
    assessment_id INTEGER,               -- References assessment table
    -- Status
    is_active BOOLEAN DEFAULT TRUE,
    -- Metadata
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    -- Constraint for content type
    CONSTRAINT module_content_type_check
        CHECK (content_type IN ('reading', 'video', 'quiz', 'interactive', 'document')),
    -- Unique constraint for content ordering within a module
    UNIQUE(module_id, content_order)
);

-- =====================================================
-- STUDENT COURSE ENROLLMENT TABLE
-- =====================================================
-- Tracks which students are enrolled in which courses

CREATE TABLE IF NOT EXISTS student_course_enrollment (
    id SERIAL PRIMARY KEY,
    student_id INTEGER NOT NULL REFERENCES student(id) ON DELETE CASCADE,
    course_id INTEGER NOT NULL REFERENCES course(id) ON DELETE CASCADE,
    -- Enrollment info
    enrolled_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    enrollment_status VARCHAR(50) DEFAULT 'active',
    -- Progress tracking
    current_module_id INTEGER REFERENCES course_module(id),
    progress_percentage DOUBLE PRECISION DEFAULT 0,
    total_time_spent INTEGER DEFAULT 0,  -- Total seconds spent in course
    -- Completion
    started_at TIMESTAMP,
    completed_at TIMESTAMP,
    completion_status VARCHAR(50) DEFAULT 'not_started',
    -- Final score
    final_score DOUBLE PRECISION,
    passed BOOLEAN DEFAULT FALSE,
    -- Metadata
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    -- Each student can only enroll in each course once
    UNIQUE(student_id, course_id),
    -- Status constraints
    CONSTRAINT enrollment_status_check
        CHECK (enrollment_status IN ('active', 'paused', 'withdrawn', 'completed', 'expired')),
    CONSTRAINT completion_status_check
        CHECK (completion_status IN ('not_started', 'in_progress', 'completed', 'failed'))
);

-- =====================================================
-- STUDENT MODULE PROGRESS TABLE
-- =====================================================
-- Tracks student progress through individual modules

CREATE TABLE IF NOT EXISTS student_module_progress (
    id SERIAL PRIMARY KEY,
    student_id INTEGER NOT NULL REFERENCES student(id) ON DELETE CASCADE,
    module_id INTEGER NOT NULL REFERENCES course_module(id) ON DELETE CASCADE,
    enrollment_id INTEGER NOT NULL REFERENCES student_course_enrollment(id) ON DELETE CASCADE,
    -- Progress tracking
    status VARCHAR(50) DEFAULT 'not_started',
    progress_percentage DOUBLE PRECISION DEFAULT 0,
    time_spent INTEGER DEFAULT 0,  -- Seconds spent in module
    -- Content completion tracking (JSON array of completed content IDs)
    completed_content_ids JSONB DEFAULT '[]',
    -- Attempts and scoring
    attempts INTEGER DEFAULT 0,
    best_score DOUBLE PRECISION,
    last_score DOUBLE PRECISION,
    -- Timestamps
    started_at TIMESTAMP,
    completed_at TIMESTAMP,
    last_accessed_at TIMESTAMP,
    -- Metadata
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    -- Each student can only have one progress record per module
    UNIQUE(student_id, module_id),
    -- Status constraint
    CONSTRAINT module_progress_status_check
        CHECK (status IN ('not_started', 'in_progress', 'completed', 'failed', 'locked'))
);

-- =====================================================
-- STUDENT CONTENT PROGRESS TABLE
-- =====================================================
-- Tracks student progress through individual content items

CREATE TABLE IF NOT EXISTS student_content_progress (
    id SERIAL PRIMARY KEY,
    student_id INTEGER NOT NULL REFERENCES student(id) ON DELETE CASCADE,
    content_id INTEGER NOT NULL REFERENCES module_content(id) ON DELETE CASCADE,
    module_progress_id INTEGER NOT NULL REFERENCES student_module_progress(id) ON DELETE CASCADE,
    -- Progress tracking
    status VARCHAR(50) DEFAULT 'not_started',
    time_spent INTEGER DEFAULT 0,  -- Seconds
    -- For video content
    last_position INTEGER DEFAULT 0,  -- Seconds into video
    view_count INTEGER DEFAULT 0,
    -- Completion
    completed BOOLEAN DEFAULT FALSE,
    completed_at TIMESTAMP,
    -- Metadata
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    -- Each student can only have one progress record per content
    UNIQUE(student_id, content_id),
    -- Status constraint
    CONSTRAINT content_progress_status_check
        CHECK (status IN ('not_started', 'in_progress', 'completed'))
);

-- =====================================================
-- STUDENT TIME LOG TABLE
-- =====================================================
-- Detailed time tracking for regulatory compliance
-- Records each session a student spends in the course

CREATE TABLE IF NOT EXISTS student_time_log (
    id SERIAL PRIMARY KEY,
    student_id INTEGER NOT NULL REFERENCES student(id) ON DELETE CASCADE,
    enrollment_id INTEGER NOT NULL REFERENCES student_course_enrollment(id) ON DELETE CASCADE,
    -- What they were working on
    course_id INTEGER REFERENCES course(id),
    module_id INTEGER REFERENCES course_module(id),
    content_id INTEGER REFERENCES module_content(id),
    -- Session info
    session_start TIMESTAMP NOT NULL,
    session_end TIMESTAMP,
    duration_seconds INTEGER DEFAULT 0,
    -- Activity type
    activity_type VARCHAR(50) NOT NULL,  -- 'reading', 'video', 'quiz', 'review'
    -- Validation
    is_validated BOOLEAN DEFAULT FALSE,
    validation_method VARCHAR(50),  -- 'automatic', 'proctor', 'manual'
    -- Client info (for verification)
    ip_address VARCHAR(50),
    user_agent VARCHAR(500),
    -- Metadata
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    -- Activity type constraint
    CONSTRAINT time_log_activity_check
        CHECK (activity_type IN ('reading', 'video', 'quiz', 'review', 'navigation', 'idle'))
);

-- =====================================================
-- ASSESSMENT TABLE
-- =====================================================
-- Quiz and exam definitions

CREATE TABLE IF NOT EXISTS assessment (
    id SERIAL PRIMARY KEY,
    course_id INTEGER REFERENCES course(id) ON DELETE CASCADE,
    module_id INTEGER REFERENCES course_module(id) ON DELETE CASCADE,
    -- Assessment info
    code VARCHAR(50) NOT NULL,
    title VARCHAR(200) NOT NULL,
    description TEXT,
    instructions TEXT,
    -- Type and configuration
    assessment_type VARCHAR(50) NOT NULL,  -- 'quiz', 'module_exam', 'final_exam'
    question_count INTEGER DEFAULT 0,
    passing_score INTEGER DEFAULT 70,
    time_limit_minutes INTEGER DEFAULT 0,  -- 0 = no limit
    -- Attempt rules
    max_attempts INTEGER DEFAULT 3,
    allow_review BOOLEAN DEFAULT TRUE,
    shuffle_questions BOOLEAN DEFAULT TRUE,
    shuffle_answers BOOLEAN DEFAULT TRUE,
    show_correct_answers BOOLEAN DEFAULT FALSE,
    -- Availability
    is_active BOOLEAN DEFAULT TRUE,
    available_from TIMESTAMP,
    available_until TIMESTAMP,
    -- Metadata
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    -- Assessment type constraint
    CONSTRAINT assessment_type_check
        CHECK (assessment_type IN ('quiz', 'module_exam', 'final_exam', 'practice'))
);

-- Add foreign key from module_content to assessment
ALTER TABLE module_content
    ADD CONSTRAINT module_content_assessment_fk
    FOREIGN KEY (assessment_id) REFERENCES assessment(id) ON DELETE SET NULL;

-- =====================================================
-- ASSESSMENT QUESTION TABLE
-- =====================================================
-- Individual questions for assessments

CREATE TABLE IF NOT EXISTS assessment_question (
    id SERIAL PRIMARY KEY,
    assessment_id INTEGER NOT NULL REFERENCES assessment(id) ON DELETE CASCADE,
    question_order INTEGER NOT NULL,
    -- Question content
    question_text TEXT NOT NULL,
    question_type VARCHAR(50) NOT NULL,  -- 'multiple_choice', 'true_false', 'multiple_select'
    -- Answer options (JSON array)
    answer_options JSONB NOT NULL DEFAULT '[]',
    -- Correct answer(s)
    correct_answer VARCHAR(500),          -- For single answer (multiple_choice, true_false)
    correct_answers JSONB DEFAULT '[]',   -- For multiple answers (multiple_select)
    -- Explanation
    explanation TEXT,
    -- Point value
    points INTEGER DEFAULT 1,
    -- Metadata
    is_active BOOLEAN DEFAULT TRUE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    -- Question type constraint
    CONSTRAINT question_type_check
        CHECK (question_type IN ('multiple_choice', 'true_false', 'multiple_select', 'short_answer'))
);

-- =====================================================
-- STUDENT ASSESSMENT ATTEMPT TABLE
-- =====================================================
-- Records each attempt a student makes at an assessment

CREATE TABLE IF NOT EXISTS student_assessment_attempt (
    id SERIAL PRIMARY KEY,
    student_id INTEGER NOT NULL REFERENCES student(id) ON DELETE CASCADE,
    assessment_id INTEGER NOT NULL REFERENCES assessment(id) ON DELETE CASCADE,
    enrollment_id INTEGER NOT NULL REFERENCES student_course_enrollment(id) ON DELETE CASCADE,
    -- Attempt info
    attempt_number INTEGER NOT NULL,
    -- Timing
    started_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    submitted_at TIMESTAMP,
    time_spent_seconds INTEGER DEFAULT 0,
    -- Results
    status VARCHAR(50) DEFAULT 'in_progress',
    total_questions INTEGER DEFAULT 0,
    correct_answers INTEGER DEFAULT 0,
    score DOUBLE PRECISION,
    passed BOOLEAN DEFAULT FALSE,
    -- Review data
    reviewed_at TIMESTAMP,
    reviewed_by INTEGER REFERENCES admin_user(id),
    review_notes TEXT,
    -- Client info
    ip_address VARCHAR(50),
    user_agent VARCHAR(500),
    -- Metadata
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    -- Unique attempt number per student per assessment
    UNIQUE(student_id, assessment_id, attempt_number),
    -- Status constraint
    CONSTRAINT attempt_status_check
        CHECK (status IN ('in_progress', 'submitted', 'graded', 'expired', 'abandoned'))
);

-- =====================================================
-- STUDENT ASSESSMENT ANSWER TABLE
-- =====================================================
-- Individual answers given by students

CREATE TABLE IF NOT EXISTS student_assessment_answer (
    id SERIAL PRIMARY KEY,
    attempt_id INTEGER NOT NULL REFERENCES student_assessment_attempt(id) ON DELETE CASCADE,
    question_id INTEGER NOT NULL REFERENCES assessment_question(id) ON DELETE CASCADE,
    -- Answer data
    answer_given VARCHAR(500),
    answers_given JSONB DEFAULT '[]',  -- For multiple select
    -- Grading
    is_correct BOOLEAN,
    points_earned DOUBLE PRECISION DEFAULT 0,
    -- Timing
    answered_at TIMESTAMP,
    time_spent_seconds INTEGER DEFAULT 0,
    -- Metadata
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    -- Each question can only be answered once per attempt
    UNIQUE(attempt_id, question_id)
);

-- =====================================================
-- ASSESSMENT REPORT TABLE
-- =====================================================
-- Final assessment reports generated upon course completion

CREATE TABLE IF NOT EXISTS assessment_report (
    id SERIAL PRIMARY KEY,
    student_id INTEGER NOT NULL REFERENCES student(id) ON DELETE CASCADE,
    enrollment_id INTEGER NOT NULL REFERENCES student_course_enrollment(id) ON DELETE CASCADE,
    course_id INTEGER NOT NULL REFERENCES course(id) ON DELETE CASCADE,
    -- Report info
    report_number VARCHAR(50) UNIQUE,
    report_type VARCHAR(50) DEFAULT 'completion',
    -- Student info snapshot
    student_name VARCHAR(200),
    student_email VARCHAR(200),
    -- Course info snapshot
    course_name VARCHAR(200),
    total_modules INTEGER,
    -- Completion summary
    modules_completed INTEGER DEFAULT 0,
    total_time_hours DOUBLE PRECISION DEFAULT 0,
    -- Score summary
    overall_score DOUBLE PRECISION,
    final_exam_score DOUBLE PRECISION,
    average_quiz_score DOUBLE PRECISION,
    -- Status
    passed BOOLEAN DEFAULT FALSE,
    completion_date DATE,
    -- Certificate info
    certificate_issued BOOLEAN DEFAULT FALSE,
    certificate_number VARCHAR(100),
    certificate_issued_at TIMESTAMP,
    -- Module breakdown (JSON array)
    module_scores JSONB DEFAULT '[]',
    -- Detailed time log summary (JSON)
    time_log_summary JSONB DEFAULT '{}',
    -- Metadata
    generated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    generated_by INTEGER REFERENCES admin_user(id),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    -- Report type constraint
    CONSTRAINT report_type_check
        CHECK (report_type IN ('completion', 'progress', 'withdrawal', 'failure'))
);

-- =====================================================
-- INDEXES FOR PERFORMANCE
-- =====================================================

-- Course indexes
CREATE INDEX IF NOT EXISTS idx_course_curriculum ON course(curriculum_id);
CREATE INDEX IF NOT EXISTS idx_course_active ON course(is_active);

-- Module indexes
CREATE INDEX IF NOT EXISTS idx_module_course ON course_module(course_id);
CREATE INDEX IF NOT EXISTS idx_module_order ON course_module(course_id, module_number);

-- Content indexes
CREATE INDEX IF NOT EXISTS idx_content_module ON module_content(module_id);
CREATE INDEX IF NOT EXISTS idx_content_order ON module_content(module_id, content_order);
CREATE INDEX IF NOT EXISTS idx_content_type ON module_content(content_type);

-- Enrollment indexes
CREATE INDEX IF NOT EXISTS idx_enrollment_student ON student_course_enrollment(student_id);
CREATE INDEX IF NOT EXISTS idx_enrollment_course ON student_course_enrollment(course_id);
CREATE INDEX IF NOT EXISTS idx_enrollment_status ON student_course_enrollment(enrollment_status);

-- Progress indexes
CREATE INDEX IF NOT EXISTS idx_module_progress_student ON student_module_progress(student_id);
CREATE INDEX IF NOT EXISTS idx_module_progress_module ON student_module_progress(module_id);
CREATE INDEX IF NOT EXISTS idx_module_progress_enrollment ON student_module_progress(enrollment_id);

CREATE INDEX IF NOT EXISTS idx_content_progress_student ON student_content_progress(student_id);
CREATE INDEX IF NOT EXISTS idx_content_progress_content ON student_content_progress(content_id);

-- Time log indexes
CREATE INDEX IF NOT EXISTS idx_time_log_student ON student_time_log(student_id);
CREATE INDEX IF NOT EXISTS idx_time_log_enrollment ON student_time_log(enrollment_id);
CREATE INDEX IF NOT EXISTS idx_time_log_session ON student_time_log(session_start, session_end);
CREATE INDEX IF NOT EXISTS idx_time_log_course ON student_time_log(course_id);

-- Assessment indexes
CREATE INDEX IF NOT EXISTS idx_assessment_course ON assessment(course_id);
CREATE INDEX IF NOT EXISTS idx_assessment_module ON assessment(module_id);
CREATE INDEX IF NOT EXISTS idx_assessment_type ON assessment(assessment_type);

-- Question indexes
CREATE INDEX IF NOT EXISTS idx_question_assessment ON assessment_question(assessment_id);

-- Attempt indexes
CREATE INDEX IF NOT EXISTS idx_attempt_student ON student_assessment_attempt(student_id);
CREATE INDEX IF NOT EXISTS idx_attempt_assessment ON student_assessment_attempt(assessment_id);
CREATE INDEX IF NOT EXISTS idx_attempt_status ON student_assessment_attempt(status);

-- Answer indexes
CREATE INDEX IF NOT EXISTS idx_answer_attempt ON student_assessment_answer(attempt_id);

-- Report indexes
CREATE INDEX IF NOT EXISTS idx_report_student ON assessment_report(student_id);
CREATE INDEX IF NOT EXISTS idx_report_course ON assessment_report(course_id);
CREATE INDEX IF NOT EXISTS idx_report_enrollment ON assessment_report(enrollment_id);

-- =====================================================
-- TRIGGERS: Auto-update updated_at column
-- =====================================================

DROP TRIGGER IF EXISTS update_course_updated_at ON course;
CREATE TRIGGER update_course_updated_at BEFORE UPDATE ON course FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

DROP TRIGGER IF EXISTS update_course_module_updated_at ON course_module;
CREATE TRIGGER update_course_module_updated_at BEFORE UPDATE ON course_module FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

DROP TRIGGER IF EXISTS update_module_content_updated_at ON module_content;
CREATE TRIGGER update_module_content_updated_at BEFORE UPDATE ON module_content FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

DROP TRIGGER IF EXISTS update_student_course_enrollment_updated_at ON student_course_enrollment;
CREATE TRIGGER update_student_course_enrollment_updated_at BEFORE UPDATE ON student_course_enrollment FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

DROP TRIGGER IF EXISTS update_student_module_progress_updated_at ON student_module_progress;
CREATE TRIGGER update_student_module_progress_updated_at BEFORE UPDATE ON student_module_progress FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

DROP TRIGGER IF EXISTS update_student_content_progress_updated_at ON student_content_progress;
CREATE TRIGGER update_student_content_progress_updated_at BEFORE UPDATE ON student_content_progress FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

DROP TRIGGER IF EXISTS update_student_time_log_updated_at ON student_time_log;
CREATE TRIGGER update_student_time_log_updated_at BEFORE UPDATE ON student_time_log FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

DROP TRIGGER IF EXISTS update_assessment_updated_at ON assessment;
CREATE TRIGGER update_assessment_updated_at BEFORE UPDATE ON assessment FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

DROP TRIGGER IF EXISTS update_assessment_question_updated_at ON assessment_question;
CREATE TRIGGER update_assessment_question_updated_at BEFORE UPDATE ON assessment_question FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

DROP TRIGGER IF EXISTS update_student_assessment_attempt_updated_at ON student_assessment_attempt;
CREATE TRIGGER update_student_assessment_attempt_updated_at BEFORE UPDATE ON student_assessment_attempt FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

DROP TRIGGER IF EXISTS update_student_assessment_answer_updated_at ON student_assessment_answer;
CREATE TRIGGER update_student_assessment_answer_updated_at BEFORE UPDATE ON student_assessment_answer FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

DROP TRIGGER IF EXISTS update_assessment_report_updated_at ON assessment_report;
CREATE TRIGGER update_assessment_report_updated_at BEFORE UPDATE ON assessment_report FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

-- =====================================================
-- END OF MIGRATION 013
-- =====================================================

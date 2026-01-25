-- =============================================================================
-- Migration 014: Add Instructor Feature
-- =============================================================================
-- Adds support for CDL instructors and certified inspectors who can:
-- - Observe student progress
-- - Provide feedback
-- - Schedule driver's skills practice and pre-trip training
-- - Validate student achievements
-- - Issue CDL certifications (for certified inspectors)
--
-- Instructor Types:
-- - CDL Instructor: Qualified to teach and train CDL students
-- - CDL Inspector/Examiner: Certified to test and issue CDL licenses
-- - Both: Can perform all functions
-- =============================================================================

-- =====================================================
-- INSTRUCTOR TABLE
-- =====================================================
-- Extends admin_user with instructor-specific information

CREATE TABLE IF NOT EXISTS instructor (
    id SERIAL PRIMARY KEY,
    admin_user_id INTEGER NOT NULL REFERENCES admin_user(id) ON DELETE CASCADE,
    -- Personal info
    employee_id VARCHAR(50) UNIQUE,
    hire_date DATE,
    -- Instructor type
    instructor_type VARCHAR(50) NOT NULL DEFAULT 'instructor',
    -- CDL specific
    cdl_number VARCHAR(50),
    cdl_class VARCHAR(10),
    cdl_state VARCHAR(50),
    cdl_expiration DATE,
    -- Inspector/Examiner certification
    is_certified_examiner BOOLEAN DEFAULT FALSE,
    examiner_certification_number VARCHAR(100),
    examiner_certification_expiration DATE,
    examiner_issuing_authority VARCHAR(200),
    -- Endorsements the instructor can teach/test
    endorsed_classes JSONB DEFAULT '[]',  -- ["A", "B", "C"]
    endorsed_endorsements JSONB DEFAULT '[]',  -- ["H", "N", "P", "S", "T"]
    -- Status
    is_active BOOLEAN DEFAULT TRUE,
    can_schedule BOOLEAN DEFAULT TRUE,
    can_validate BOOLEAN DEFAULT TRUE,
    can_issue_cdl BOOLEAN DEFAULT FALSE,
    -- Metadata
    notes TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    -- Instructor type constraint
    CONSTRAINT instructor_type_check
        CHECK (instructor_type IN ('instructor', 'examiner', 'both'))
);

-- =====================================================
-- INSTRUCTOR QUALIFICATION TABLE
-- =====================================================
-- Tracks instructor qualifications and certifications

CREATE TABLE IF NOT EXISTS instructor_qualification (
    id SERIAL PRIMARY KEY,
    instructor_id INTEGER NOT NULL REFERENCES instructor(id) ON DELETE CASCADE,
    -- Qualification info
    qualification_type VARCHAR(100) NOT NULL,
    qualification_name VARCHAR(200) NOT NULL,
    issuing_authority VARCHAR(200),
    certification_number VARCHAR(100),
    -- Dates
    issued_date DATE,
    expiration_date DATE,
    -- Status
    is_active BOOLEAN DEFAULT TRUE,
    is_verified BOOLEAN DEFAULT FALSE,
    verified_by INTEGER REFERENCES admin_user(id),
    verified_at TIMESTAMP,
    -- Documents
    document_path VARCHAR(500),
    -- Metadata
    notes TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    -- Unique qualification per instructor
    UNIQUE(instructor_id, qualification_type, certification_number)
);

-- =====================================================
-- INSTRUCTOR ASSIGNMENT TABLE
-- =====================================================
-- Assigns instructors to specific students or courses

CREATE TABLE IF NOT EXISTS instructor_assignment (
    id SERIAL PRIMARY KEY,
    instructor_id INTEGER NOT NULL REFERENCES instructor(id) ON DELETE CASCADE,
    -- Assignment target (student OR course, not both)
    student_id INTEGER REFERENCES student(id) ON DELETE CASCADE,
    course_id INTEGER REFERENCES course(id) ON DELETE CASCADE,
    enrollment_id INTEGER REFERENCES student_course_enrollment(id) ON DELETE CASCADE,
    -- Assignment type
    assignment_type VARCHAR(50) NOT NULL,  -- 'primary', 'secondary', 'examiner'
    -- Scope
    assignment_scope VARCHAR(50) DEFAULT 'full',  -- 'full', 'classroom', 'range', 'road', 'exam'
    -- Dates
    assigned_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    effective_from DATE,
    effective_until DATE,
    -- Status
    is_active BOOLEAN DEFAULT TRUE,
    -- Metadata
    assigned_by INTEGER REFERENCES admin_user(id),
    notes TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    -- Constraints
    CONSTRAINT assignment_target_check
        CHECK (student_id IS NOT NULL OR course_id IS NOT NULL),
    CONSTRAINT assignment_type_check
        CHECK (assignment_type IN ('primary', 'secondary', 'backup', 'examiner', 'observer'))
);

-- =====================================================
-- SCHEDULED SESSION TABLE
-- =====================================================
-- Tracks scheduled training sessions (skills practice, pre-trip, road test, etc.)

CREATE TABLE IF NOT EXISTS scheduled_session (
    id SERIAL PRIMARY KEY,
    instructor_id INTEGER NOT NULL REFERENCES instructor(id) ON DELETE CASCADE,
    student_id INTEGER NOT NULL REFERENCES student(id) ON DELETE CASCADE,
    enrollment_id INTEGER REFERENCES student_course_enrollment(id) ON DELETE CASCADE,
    -- Session info
    session_type VARCHAR(50) NOT NULL,
    title VARCHAR(200) NOT NULL,
    description TEXT,
    -- Scheduling
    scheduled_date DATE NOT NULL,
    start_time TIME NOT NULL,
    end_time TIME,
    duration_minutes INTEGER DEFAULT 60,
    -- Location
    location_type VARCHAR(50),  -- 'classroom', 'range', 'road', 'virtual', 'other'
    location_name VARCHAR(200),
    location_address TEXT,
    -- Vehicle (if applicable)
    vehicle_id INTEGER,
    vehicle_info VARCHAR(200),
    -- Status
    status VARCHAR(50) DEFAULT 'scheduled',
    -- Completion
    actual_start_time TIMESTAMP,
    actual_end_time TIMESTAMP,
    actual_duration_minutes INTEGER,
    completed_at TIMESTAMP,
    -- Cancellation
    cancelled_at TIMESTAMP,
    cancelled_by INTEGER REFERENCES admin_user(id),
    cancellation_reason TEXT,
    -- Rescheduling
    rescheduled_from INTEGER REFERENCES scheduled_session(id),
    reschedule_count INTEGER DEFAULT 0,
    -- Metadata
    created_by INTEGER REFERENCES admin_user(id),
    notes TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    -- Status constraint
    CONSTRAINT session_status_check
        CHECK (status IN ('scheduled', 'confirmed', 'in_progress', 'completed', 'cancelled', 'no_show', 'rescheduled')),
    -- Session type constraint
    CONSTRAINT session_type_check
        CHECK (session_type IN (
            'orientation', 'classroom', 'pre_trip_inspection', 'basic_control',
            'range_practice', 'road_practice', 'skills_test', 'road_test',
            'final_exam', 'remedial', 'evaluation', 'other'
        ))
);

-- =====================================================
-- SESSION ATTENDANCE TABLE
-- =====================================================
-- Tracks attendance for scheduled sessions

CREATE TABLE IF NOT EXISTS session_attendance (
    id SERIAL PRIMARY KEY,
    session_id INTEGER NOT NULL REFERENCES scheduled_session(id) ON DELETE CASCADE,
    student_id INTEGER NOT NULL REFERENCES student(id) ON DELETE CASCADE,
    -- Attendance
    status VARCHAR(50) DEFAULT 'expected',
    check_in_time TIMESTAMP,
    check_out_time TIMESTAMP,
    -- Verification
    verified_by_instructor BOOLEAN DEFAULT FALSE,
    instructor_signature VARCHAR(500),
    student_signature VARCHAR(500),
    -- Metadata
    notes TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    -- Unique attendance per student per session
    UNIQUE(session_id, student_id),
    -- Status constraint
    CONSTRAINT attendance_status_check
        CHECK (status IN ('expected', 'present', 'absent', 'late', 'excused', 'no_show'))
);

-- =====================================================
-- STUDENT FEEDBACK TABLE
-- =====================================================
-- Instructor feedback on student performance

CREATE TABLE IF NOT EXISTS student_feedback (
    id SERIAL PRIMARY KEY,
    instructor_id INTEGER NOT NULL REFERENCES instructor(id) ON DELETE CASCADE,
    student_id INTEGER NOT NULL REFERENCES student(id) ON DELETE CASCADE,
    enrollment_id INTEGER REFERENCES student_course_enrollment(id) ON DELETE CASCADE,
    session_id INTEGER REFERENCES scheduled_session(id) ON DELETE SET NULL,
    -- Feedback context
    feedback_type VARCHAR(50) NOT NULL,
    feedback_category VARCHAR(100),
    -- Content
    subject VARCHAR(200),
    feedback_text TEXT NOT NULL,
    -- Rating (optional)
    performance_rating INTEGER,  -- 1-5 or 1-10
    rating_scale INTEGER DEFAULT 5,
    -- Areas
    strengths TEXT,
    areas_for_improvement TEXT,
    recommended_actions TEXT,
    -- Visibility
    is_private BOOLEAN DEFAULT FALSE,  -- If true, only visible to staff
    visible_to_student BOOLEAN DEFAULT TRUE,
    -- Acknowledgment
    acknowledged_by_student BOOLEAN DEFAULT FALSE,
    acknowledged_at TIMESTAMP,
    -- Follow-up
    requires_follow_up BOOLEAN DEFAULT FALSE,
    follow_up_date DATE,
    follow_up_completed BOOLEAN DEFAULT FALSE,
    -- Metadata
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    -- Feedback type constraint
    CONSTRAINT feedback_type_check
        CHECK (feedback_type IN (
            'progress_update', 'session_review', 'skill_assessment',
            'behavior', 'safety_concern', 'commendation', 'general', 'other'
        ))
);

-- =====================================================
-- SKILL CATEGORY TABLE
-- =====================================================
-- Defines skill categories for CDL training

CREATE TABLE IF NOT EXISTS skill_category (
    id SERIAL PRIMARY KEY,
    code VARCHAR(50) NOT NULL UNIQUE,
    name VARCHAR(200) NOT NULL,
    description TEXT,
    category_type VARCHAR(50) NOT NULL,  -- 'pre_trip', 'basic_control', 'road', 'general'
    -- Requirements
    minimum_practice_hours DOUBLE PRECISION DEFAULT 0,
    requires_validation BOOLEAN DEFAULT TRUE,
    -- Ordering
    display_order INTEGER DEFAULT 0,
    parent_category_id INTEGER REFERENCES skill_category(id),
    -- Status
    is_active BOOLEAN DEFAULT TRUE,
    -- Metadata
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- =====================================================
-- SKILL ITEM TABLE
-- =====================================================
-- Specific skills within a category

CREATE TABLE IF NOT EXISTS skill_item (
    id SERIAL PRIMARY KEY,
    category_id INTEGER NOT NULL REFERENCES skill_category(id) ON DELETE CASCADE,
    code VARCHAR(50) NOT NULL,
    name VARCHAR(200) NOT NULL,
    description TEXT,
    -- Grading criteria
    passing_criteria TEXT,
    critical_errors TEXT,  -- Errors that result in automatic failure
    point_value INTEGER DEFAULT 1,
    -- Requirements
    requires_demonstration BOOLEAN DEFAULT TRUE,
    minimum_successful_attempts INTEGER DEFAULT 1,
    -- Ordering
    display_order INTEGER DEFAULT 0,
    -- Status
    is_active BOOLEAN DEFAULT TRUE,
    -- Metadata
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    -- Unique skill per category
    UNIQUE(category_id, code)
);

-- =====================================================
-- SKILL VALIDATION TABLE
-- =====================================================
-- Records when instructors validate student skills

CREATE TABLE IF NOT EXISTS skill_validation (
    id SERIAL PRIMARY KEY,
    instructor_id INTEGER NOT NULL REFERENCES instructor(id) ON DELETE CASCADE,
    student_id INTEGER NOT NULL REFERENCES student(id) ON DELETE CASCADE,
    enrollment_id INTEGER REFERENCES student_course_enrollment(id) ON DELETE CASCADE,
    skill_item_id INTEGER NOT NULL REFERENCES skill_item(id) ON DELETE CASCADE,
    session_id INTEGER REFERENCES scheduled_session(id) ON DELETE SET NULL,
    -- Validation result
    validation_type VARCHAR(50) NOT NULL,  -- 'practice', 'evaluation', 'test'
    result VARCHAR(50) NOT NULL,  -- 'pass', 'fail', 'needs_practice', 'deferred'
    attempt_number INTEGER DEFAULT 1,
    -- Scoring
    score DOUBLE PRECISION,
    max_score DOUBLE PRECISION,
    points_earned DOUBLE PRECISION,
    -- Errors/Deductions
    errors_noted TEXT,
    deductions JSONB DEFAULT '[]',  -- [{error: "", points: 0}]
    critical_error BOOLEAN DEFAULT FALSE,
    -- Timestamps
    validated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    -- Verification (for official tests)
    is_official_test BOOLEAN DEFAULT FALSE,
    witness_name VARCHAR(200),
    witness_signature VARCHAR(500),
    -- Digital signature
    instructor_signature VARCHAR(500),
    student_signature VARCHAR(500),
    -- Metadata
    notes TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    -- Result constraint
    CONSTRAINT validation_result_check
        CHECK (result IN ('pass', 'fail', 'needs_practice', 'deferred', 'incomplete'))
);

-- =====================================================
-- STUDENT SKILL PROGRESS TABLE
-- =====================================================
-- Aggregated skill progress for students

CREATE TABLE IF NOT EXISTS student_skill_progress (
    id SERIAL PRIMARY KEY,
    student_id INTEGER NOT NULL REFERENCES student(id) ON DELETE CASCADE,
    enrollment_id INTEGER REFERENCES student_course_enrollment(id) ON DELETE CASCADE,
    skill_item_id INTEGER NOT NULL REFERENCES skill_item(id) ON DELETE CASCADE,
    -- Progress
    status VARCHAR(50) DEFAULT 'not_started',
    practice_count INTEGER DEFAULT 0,
    successful_count INTEGER DEFAULT 0,
    failed_count INTEGER DEFAULT 0,
    -- Best result
    best_score DOUBLE PRECISION,
    best_validation_id INTEGER REFERENCES skill_validation(id),
    -- Completion
    is_validated BOOLEAN DEFAULT FALSE,
    validated_at TIMESTAMP,
    validated_by INTEGER REFERENCES instructor(id),
    -- Hours
    practice_hours DOUBLE PRECISION DEFAULT 0,
    -- Metadata
    last_practiced_at TIMESTAMP,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    -- Unique progress per student per skill
    UNIQUE(student_id, skill_item_id),
    -- Status constraint
    CONSTRAINT skill_progress_status_check
        CHECK (status IN ('not_started', 'in_progress', 'needs_practice', 'validated', 'expired'))
);

-- =====================================================
-- CDL TEST RESULT TABLE
-- =====================================================
-- Official CDL test results (for certified examiners)

CREATE TABLE IF NOT EXISTS cdl_test_result (
    id SERIAL PRIMARY KEY,
    examiner_id INTEGER NOT NULL REFERENCES instructor(id) ON DELETE CASCADE,
    student_id INTEGER NOT NULL REFERENCES student(id) ON DELETE CASCADE,
    enrollment_id INTEGER REFERENCES student_course_enrollment(id) ON DELETE CASCADE,
    -- Test info
    test_type VARCHAR(50) NOT NULL,  -- 'knowledge', 'pre_trip', 'basic_control', 'road'
    test_date DATE NOT NULL,
    test_location VARCHAR(200),
    -- Vehicle
    vehicle_type VARCHAR(50),
    vehicle_class VARCHAR(10),
    -- Results
    result VARCHAR(50) NOT NULL,
    score DOUBLE PRECISION,
    passing_score DOUBLE PRECISION,
    -- Detailed scores (JSON)
    section_scores JSONB DEFAULT '{}',
    -- Errors
    errors_noted TEXT,
    automatic_failures JSONB DEFAULT '[]',
    -- Official documentation
    test_form_number VARCHAR(100),
    certificate_number VARCHAR(100),
    -- Signatures
    examiner_signature VARCHAR(500),
    student_signature VARCHAR(500),
    examiner_certification_number VARCHAR(100),
    -- Verification
    is_verified BOOLEAN DEFAULT FALSE,
    verified_by INTEGER REFERENCES admin_user(id),
    verified_at TIMESTAMP,
    -- Metadata
    notes TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    -- Result constraint
    CONSTRAINT cdl_test_result_check
        CHECK (result IN ('pass', 'fail', 'incomplete', 'deferred'))
);

-- =====================================================
-- INSTRUCTOR AVAILABILITY TABLE
-- =====================================================
-- Tracks instructor availability for scheduling

CREATE TABLE IF NOT EXISTS instructor_availability (
    id SERIAL PRIMARY KEY,
    instructor_id INTEGER NOT NULL REFERENCES instructor(id) ON DELETE CASCADE,
    -- Recurring availability
    day_of_week INTEGER,  -- 0=Sunday, 6=Saturday (NULL for specific date)
    specific_date DATE,   -- For one-time availability
    -- Time slots
    start_time TIME NOT NULL,
    end_time TIME NOT NULL,
    -- Type
    availability_type VARCHAR(50) DEFAULT 'available',
    -- Location preference
    preferred_location VARCHAR(200),
    -- Metadata
    notes TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    -- Availability type constraint
    CONSTRAINT availability_type_check
        CHECK (availability_type IN ('available', 'unavailable', 'tentative', 'blocked'))
);

-- =====================================================
-- INDEXES FOR PERFORMANCE
-- =====================================================

-- Instructor indexes
CREATE INDEX IF NOT EXISTS idx_instructor_admin_user ON instructor(admin_user_id);
CREATE INDEX IF NOT EXISTS idx_instructor_type ON instructor(instructor_type);
CREATE INDEX IF NOT EXISTS idx_instructor_examiner ON instructor(is_certified_examiner);
CREATE INDEX IF NOT EXISTS idx_instructor_active ON instructor(is_active);

-- Qualification indexes
CREATE INDEX IF NOT EXISTS idx_qualification_instructor ON instructor_qualification(instructor_id);
CREATE INDEX IF NOT EXISTS idx_qualification_type ON instructor_qualification(qualification_type);
CREATE INDEX IF NOT EXISTS idx_qualification_expiration ON instructor_qualification(expiration_date);

-- Assignment indexes
CREATE INDEX IF NOT EXISTS idx_assignment_instructor ON instructor_assignment(instructor_id);
CREATE INDEX IF NOT EXISTS idx_assignment_student ON instructor_assignment(student_id);
CREATE INDEX IF NOT EXISTS idx_assignment_course ON instructor_assignment(course_id);
CREATE INDEX IF NOT EXISTS idx_assignment_active ON instructor_assignment(is_active);

-- Session indexes
CREATE INDEX IF NOT EXISTS idx_session_instructor ON scheduled_session(instructor_id);
CREATE INDEX IF NOT EXISTS idx_session_student ON scheduled_session(student_id);
CREATE INDEX IF NOT EXISTS idx_session_date ON scheduled_session(scheduled_date);
CREATE INDEX IF NOT EXISTS idx_session_status ON scheduled_session(status);
CREATE INDEX IF NOT EXISTS idx_session_type ON scheduled_session(session_type);

-- Attendance indexes
CREATE INDEX IF NOT EXISTS idx_attendance_session ON session_attendance(session_id);
CREATE INDEX IF NOT EXISTS idx_attendance_student ON session_attendance(student_id);

-- Feedback indexes
CREATE INDEX IF NOT EXISTS idx_feedback_instructor ON student_feedback(instructor_id);
CREATE INDEX IF NOT EXISTS idx_feedback_student ON student_feedback(student_id);
CREATE INDEX IF NOT EXISTS idx_feedback_type ON student_feedback(feedback_type);
CREATE INDEX IF NOT EXISTS idx_feedback_date ON student_feedback(created_at);

-- Skill indexes
CREATE INDEX IF NOT EXISTS idx_skill_category_type ON skill_category(category_type);
CREATE INDEX IF NOT EXISTS idx_skill_item_category ON skill_item(category_id);

-- Validation indexes
CREATE INDEX IF NOT EXISTS idx_validation_instructor ON skill_validation(instructor_id);
CREATE INDEX IF NOT EXISTS idx_validation_student ON skill_validation(student_id);
CREATE INDEX IF NOT EXISTS idx_validation_skill ON skill_validation(skill_item_id);
CREATE INDEX IF NOT EXISTS idx_validation_result ON skill_validation(result);
CREATE INDEX IF NOT EXISTS idx_validation_date ON skill_validation(validated_at);

-- Progress indexes
CREATE INDEX IF NOT EXISTS idx_skill_progress_student ON student_skill_progress(student_id);
CREATE INDEX IF NOT EXISTS idx_skill_progress_skill ON student_skill_progress(skill_item_id);
CREATE INDEX IF NOT EXISTS idx_skill_progress_status ON student_skill_progress(status);

-- CDL test indexes
CREATE INDEX IF NOT EXISTS idx_cdl_test_examiner ON cdl_test_result(examiner_id);
CREATE INDEX IF NOT EXISTS idx_cdl_test_student ON cdl_test_result(student_id);
CREATE INDEX IF NOT EXISTS idx_cdl_test_date ON cdl_test_result(test_date);
CREATE INDEX IF NOT EXISTS idx_cdl_test_type ON cdl_test_result(test_type);
CREATE INDEX IF NOT EXISTS idx_cdl_test_result ON cdl_test_result(result);

-- Availability indexes
CREATE INDEX IF NOT EXISTS idx_availability_instructor ON instructor_availability(instructor_id);
CREATE INDEX IF NOT EXISTS idx_availability_day ON instructor_availability(day_of_week);
CREATE INDEX IF NOT EXISTS idx_availability_date ON instructor_availability(specific_date);

-- =====================================================
-- TRIGGERS: Auto-update updated_at column
-- =====================================================

DROP TRIGGER IF EXISTS update_instructor_updated_at ON instructor;
CREATE TRIGGER update_instructor_updated_at BEFORE UPDATE ON instructor FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

DROP TRIGGER IF EXISTS update_instructor_qualification_updated_at ON instructor_qualification;
CREATE TRIGGER update_instructor_qualification_updated_at BEFORE UPDATE ON instructor_qualification FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

DROP TRIGGER IF EXISTS update_instructor_assignment_updated_at ON instructor_assignment;
CREATE TRIGGER update_instructor_assignment_updated_at BEFORE UPDATE ON instructor_assignment FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

DROP TRIGGER IF EXISTS update_scheduled_session_updated_at ON scheduled_session;
CREATE TRIGGER update_scheduled_session_updated_at BEFORE UPDATE ON scheduled_session FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

DROP TRIGGER IF EXISTS update_session_attendance_updated_at ON session_attendance;
CREATE TRIGGER update_session_attendance_updated_at BEFORE UPDATE ON session_attendance FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

DROP TRIGGER IF EXISTS update_student_feedback_updated_at ON student_feedback;
CREATE TRIGGER update_student_feedback_updated_at BEFORE UPDATE ON student_feedback FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

DROP TRIGGER IF EXISTS update_skill_category_updated_at ON skill_category;
CREATE TRIGGER update_skill_category_updated_at BEFORE UPDATE ON skill_category FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

DROP TRIGGER IF EXISTS update_skill_item_updated_at ON skill_item;
CREATE TRIGGER update_skill_item_updated_at BEFORE UPDATE ON skill_item FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

DROP TRIGGER IF EXISTS update_skill_validation_updated_at ON skill_validation;
CREATE TRIGGER update_skill_validation_updated_at BEFORE UPDATE ON skill_validation FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

DROP TRIGGER IF EXISTS update_student_skill_progress_updated_at ON student_skill_progress;
CREATE TRIGGER update_student_skill_progress_updated_at BEFORE UPDATE ON student_skill_progress FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

DROP TRIGGER IF EXISTS update_cdl_test_result_updated_at ON cdl_test_result;
CREATE TRIGGER update_cdl_test_result_updated_at BEFORE UPDATE ON cdl_test_result FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

DROP TRIGGER IF EXISTS update_instructor_availability_updated_at ON instructor_availability;
CREATE TRIGGER update_instructor_availability_updated_at BEFORE UPDATE ON instructor_availability FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

-- =====================================================
-- SEED DATA: Default Skill Categories for CDL
-- =====================================================

INSERT INTO skill_category (code, name, description, category_type, minimum_practice_hours, display_order)
VALUES
    ('PRE_TRIP', 'Pre-Trip Inspection', 'Vehicle inspection before operation', 'pre_trip', 2, 1),
    ('BASIC_CONTROL', 'Basic Vehicle Control', 'Fundamental vehicle control skills', 'basic_control', 4, 2),
    ('RANGE', 'Range Maneuvers', 'Backing and maneuvering skills', 'basic_control', 6, 3),
    ('ROAD', 'Road Skills', 'On-road driving skills', 'road', 10, 4),
    ('SAFETY', 'Safety Procedures', 'Safety awareness and procedures', 'general', 1, 5)
ON CONFLICT (code) DO NOTHING;

-- Pre-Trip Inspection Skills
INSERT INTO skill_item (category_id, code, name, description, display_order)
SELECT c.id, s.code, s.name, s.description, s.display_order
FROM skill_category c
CROSS JOIN (VALUES
    ('ENGINE_COMPARTMENT', 'Engine Compartment', 'Inspect engine compartment components', 1),
    ('CAB_INTERIOR', 'Cab/Interior', 'Inspect cab and interior components', 2),
    ('EXTERNAL_LIGHTS', 'External Lights', 'Inspect all external lights', 3),
    ('STEERING', 'Steering Components', 'Inspect steering mechanism', 4),
    ('SUSPENSION', 'Suspension', 'Inspect suspension components', 5),
    ('BRAKES', 'Brake System', 'Inspect brake components', 6),
    ('WHEELS_TIRES', 'Wheels and Tires', 'Inspect wheels, rims, and tires', 7),
    ('COUPLING', 'Coupling System', 'Inspect coupling devices (if applicable)', 8)
) AS s(code, name, description, display_order)
WHERE c.code = 'PRE_TRIP'
ON CONFLICT (category_id, code) DO NOTHING;

-- Basic Control Skills
INSERT INTO skill_item (category_id, code, name, description, display_order)
SELECT c.id, s.code, s.name, s.description, s.display_order
FROM skill_category c
CROSS JOIN (VALUES
    ('STRAIGHT_BACK', 'Straight Line Backing', 'Back vehicle in a straight line', 1),
    ('OFFSET_BACK', 'Offset Backing', 'Back vehicle into offset position', 2),
    ('PARALLEL_PARK', 'Parallel Park', 'Parallel park conventional or sight-side', 3),
    ('ALLEY_DOCK', 'Alley Dock', 'Back into alley dock position', 4)
) AS s(code, name, description, display_order)
WHERE c.code = 'BASIC_CONTROL'
ON CONFLICT (category_id, code) DO NOTHING;

-- Road Skills
INSERT INTO skill_item (category_id, code, name, description, display_order)
SELECT c.id, s.code, s.name, s.description, s.display_order
FROM skill_category c
CROSS JOIN (VALUES
    ('TURNS', 'Turns', 'Left and right turns', 1),
    ('INTERSECTIONS', 'Intersections', 'Navigating intersections safely', 2),
    ('LANE_CHANGES', 'Lane Changes', 'Safe lane change procedures', 3),
    ('HIGHWAY', 'Highway Driving', 'Highway entry, driving, and exit', 4),
    ('CURVES', 'Curves', 'Negotiating curves safely', 5),
    ('RAILROAD', 'Railroad Crossings', 'Proper railroad crossing procedures', 6),
    ('BRIDGES', 'Bridges/Overpasses', 'Bridge and overpass navigation', 7),
    ('URBAN', 'Urban Driving', 'City driving skills', 8),
    ('RURAL', 'Rural Driving', 'Rural road driving skills', 9)
) AS s(code, name, description, display_order)
WHERE c.code = 'ROAD'
ON CONFLICT (category_id, code) DO NOTHING;

-- =====================================================
-- END OF MIGRATION 014
-- =====================================================

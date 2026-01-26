-- Migration: 016_activity_log.sql
-- Description: Creates activity_log table for unified audit trail across all user types
-- Date: 2026-01-26
-- Purpose: Captures "who did what, when" for Students, Instructors, and Admins

-- =============================================================================
-- DDL: Create activity_log table
-- =============================================================================

CREATE TABLE IF NOT EXISTS activity_log (
    id SERIAL PRIMARY KEY,

    -- Actor Information (who performed the action)
    actor_type VARCHAR(50) NOT NULL,
    actor_id INTEGER REFERENCES app_user(id) ON DELETE SET NULL,
    actor_name VARCHAR(200),
    actor_email VARCHAR(200),

    -- Action Information (what was done)
    action_type VARCHAR(100) NOT NULL,
    action_category VARCHAR(50) NOT NULL,
    description TEXT NOT NULL,

    -- Target Entity (what was acted upon)
    entity_type VARCHAR(100),
    entity_id VARCHAR(100),
    entity_name VARCHAR(200),

    -- Additional Context
    details JSONB DEFAULT '{}',
    severity VARCHAR(20) DEFAULT 'info',
    ip_address VARCHAR(50),
    user_agent TEXT,
    session_id VARCHAR(100),

    -- Timestamps
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,

    -- Constraints
    CONSTRAINT activity_log_actor_type_check
        CHECK (actor_type IN ('student', 'instructor', 'admin', 'system')),
    CONSTRAINT activity_log_action_category_check
        CHECK (action_category IN ('authentication', 'forms', 'profile', 'admin', 'system')),
    CONSTRAINT activity_log_severity_check
        CHECK (severity IN ('info', 'success', 'warning', 'error'))
);

-- =============================================================================
-- Indexes for performance
-- =============================================================================

-- Primary query: recent activities sorted by time
CREATE INDEX IF NOT EXISTS idx_activity_log_created_at ON activity_log(created_at DESC);

-- Filter by actor
CREATE INDEX IF NOT EXISTS idx_activity_log_actor_id ON activity_log(actor_id);
CREATE INDEX IF NOT EXISTS idx_activity_log_actor_type ON activity_log(actor_type);

-- Filter by action
CREATE INDEX IF NOT EXISTS idx_activity_log_action_category ON activity_log(action_category);
CREATE INDEX IF NOT EXISTS idx_activity_log_action_type ON activity_log(action_type);

-- Filter by entity
CREATE INDEX IF NOT EXISTS idx_activity_log_entity_type ON activity_log(entity_type);
CREATE INDEX IF NOT EXISTS idx_activity_log_entity_id ON activity_log(entity_id);

-- Composite index for dashboard queries (recent by category)
CREATE INDEX IF NOT EXISTS idx_activity_log_category_created
    ON activity_log(action_category, created_at DESC);

-- =============================================================================
-- Helper function: Log an activity
-- =============================================================================

CREATE OR REPLACE FUNCTION log_activity(
    p_actor_type VARCHAR(50),
    p_actor_id INTEGER,
    p_actor_name VARCHAR(200),
    p_actor_email VARCHAR(200),
    p_action_type VARCHAR(100),
    p_action_category VARCHAR(50),
    p_description TEXT,
    p_entity_type VARCHAR(100) DEFAULT NULL,
    p_entity_id VARCHAR(100) DEFAULT NULL,
    p_entity_name VARCHAR(200) DEFAULT NULL,
    p_details JSONB DEFAULT '{}',
    p_severity VARCHAR(20) DEFAULT 'info',
    p_ip_address VARCHAR(50) DEFAULT NULL,
    p_user_agent TEXT DEFAULT NULL,
    p_session_id VARCHAR(100) DEFAULT NULL
)
RETURNS INTEGER AS $$
DECLARE
    v_activity_id INTEGER;
BEGIN
    INSERT INTO activity_log (
        actor_type, actor_id, actor_name, actor_email,
        action_type, action_category, description,
        entity_type, entity_id, entity_name,
        details, severity, ip_address, user_agent, session_id
    ) VALUES (
        p_actor_type, p_actor_id, p_actor_name, p_actor_email,
        p_action_type, p_action_category, p_description,
        p_entity_type, p_entity_id, p_entity_name,
        p_details, p_severity, p_ip_address, p_user_agent, p_session_id
    )
    RETURNING id INTO v_activity_id;

    RETURN v_activity_id;
END;
$$ LANGUAGE plpgsql;

-- =============================================================================
-- Helper function: Get recent activities
-- =============================================================================

CREATE OR REPLACE FUNCTION get_recent_activities(
    p_limit INTEGER DEFAULT 10,
    p_actor_type VARCHAR(50) DEFAULT NULL,
    p_action_category VARCHAR(50) DEFAULT NULL
)
RETURNS TABLE (
    id INTEGER,
    actor_type VARCHAR(50),
    actor_id INTEGER,
    actor_name VARCHAR(200),
    actor_email VARCHAR(200),
    action_type VARCHAR(100),
    action_category VARCHAR(50),
    description TEXT,
    entity_type VARCHAR(100),
    entity_id VARCHAR(100),
    entity_name VARCHAR(200),
    details JSONB,
    severity VARCHAR(20),
    ip_address VARCHAR(50),
    created_at TIMESTAMP
) AS $$
BEGIN
    RETURN QUERY
    SELECT
        al.id, al.actor_type, al.actor_id, al.actor_name, al.actor_email,
        al.action_type, al.action_category, al.description,
        al.entity_type, al.entity_id, al.entity_name,
        al.details, al.severity, al.ip_address, al.created_at
    FROM activity_log al
    WHERE (p_actor_type IS NULL OR al.actor_type = p_actor_type)
      AND (p_action_category IS NULL OR al.action_category = p_action_category)
    ORDER BY al.created_at DESC
    LIMIT p_limit;
END;
$$ LANGUAGE plpgsql;

-- =============================================================================
-- View: Recent activities with formatted time
-- =============================================================================

CREATE OR REPLACE VIEW recent_activities AS
SELECT
    id,
    actor_type,
    actor_id,
    actor_name,
    actor_email,
    action_type,
    action_category,
    description,
    entity_type,
    entity_id,
    entity_name,
    details,
    severity,
    ip_address,
    created_at,
    -- Formatted relative time
    CASE
        WHEN created_at > NOW() - INTERVAL '1 minute' THEN 'Just now'
        WHEN created_at > NOW() - INTERVAL '1 hour' THEN
            EXTRACT(MINUTE FROM NOW() - created_at)::INTEGER || ' minutes ago'
        WHEN created_at > NOW() - INTERVAL '1 day' THEN
            EXTRACT(HOUR FROM NOW() - created_at)::INTEGER || ' hours ago'
        WHEN created_at > NOW() - INTERVAL '7 days' THEN
            EXTRACT(DAY FROM NOW() - created_at)::INTEGER || ' days ago'
        ELSE TO_CHAR(created_at, 'Mon DD, YYYY')
    END AS relative_time
FROM activity_log
ORDER BY created_at DESC;

-- =============================================================================
-- DML: Seed data for demonstration
-- =============================================================================

-- Insert sample activity log entries for testing/demo purposes
-- These represent a realistic timeline of system activity

DO $$
DECLARE
    v_now TIMESTAMP := CURRENT_TIMESTAMP;
BEGIN
    -- Only insert seed data if the table is empty
    IF NOT EXISTS (SELECT 1 FROM activity_log LIMIT 1) THEN

        -- System startup (oldest)
        INSERT INTO activity_log (actor_type, actor_id, actor_name, actor_email, action_type, action_category, description, severity, created_at)
        VALUES ('system', NULL, 'System', NULL, 'system_startup', 'system', 'Student Onboarding System initialized', 'info', v_now - INTERVAL '7 days');

        -- Admin login
        INSERT INTO activity_log (actor_type, actor_id, actor_name, actor_email, action_type, action_category, description, entity_type, severity, ip_address, created_at)
        VALUES ('admin', 1, 'System Administrator', 'admin@system.local', 'login_success', 'authentication', 'System Administrator logged in', 'app_user', 'success', '192.168.1.100', v_now - INTERVAL '6 days 23 hours');

        -- Admin creates curriculum
        INSERT INTO activity_log (actor_type, actor_id, actor_name, actor_email, action_type, action_category, description, entity_type, entity_id, entity_name, severity, created_at)
        VALUES ('admin', 1, 'System Administrator', 'admin@system.local', 'curriculum_created', 'admin', 'System Administrator created program: CDL Class A Training', 'curriculum', '1', 'CDL Class A Training', 'success', v_now - INTERVAL '6 days 22 hours');

        -- Student registration
        INSERT INTO activity_log (actor_type, actor_id, actor_name, actor_email, action_type, action_category, description, entity_type, entity_id, entity_name, severity, ip_address, created_at)
        VALUES ('student', 2, 'John Doe', 'john.doe@email.com', 'student_registered', 'profile', 'New student registered: John Doe', 'student', '2', 'John Doe', 'success', '192.168.1.50', v_now - INTERVAL '5 days');

        -- Student selects curriculum
        INSERT INTO activity_log (actor_type, actor_id, actor_name, actor_email, action_type, action_category, description, entity_type, entity_id, entity_name, severity, created_at)
        VALUES ('student', 2, 'John Doe', 'john.doe@email.com', 'curriculum_selected', 'profile', 'John Doe selected CDL Class A Training', 'curriculum', '1', 'CDL Class A Training', 'info', v_now - INTERVAL '5 days' + INTERVAL '30 minutes');

        -- Student submits personal info form
        INSERT INTO activity_log (actor_type, actor_id, actor_name, actor_email, action_type, action_category, description, entity_type, entity_id, entity_name, severity, created_at)
        VALUES ('student', 2, 'John Doe', 'john.doe@email.com', 'form_submitted', 'forms', 'John Doe submitted Personal Information form', 'form_submission', '1', 'Personal Information', 'success', v_now - INTERVAL '4 days 20 hours');

        -- Student submits emergency contact form
        INSERT INTO activity_log (actor_type, actor_id, actor_name, actor_email, action_type, action_category, description, entity_type, entity_id, entity_name, severity, created_at)
        VALUES ('student', 2, 'John Doe', 'john.doe@email.com', 'form_submitted', 'forms', 'John Doe submitted Emergency Contacts form', 'form_submission', '2', 'Emergency Contacts', 'success', v_now - INTERVAL '4 days 19 hours');

        -- Another student registration
        INSERT INTO activity_log (actor_type, actor_id, actor_name, actor_email, action_type, action_category, description, entity_type, entity_id, entity_name, severity, ip_address, created_at)
        VALUES ('student', 3, 'Jane Smith', 'jane.smith@email.com', 'student_registered', 'profile', 'New student registered: Jane Smith', 'student', '3', 'Jane Smith', 'success', '192.168.1.51', v_now - INTERVAL '3 days');

        -- Admin reviews form
        INSERT INTO activity_log (actor_type, actor_id, actor_name, actor_email, action_type, action_category, description, entity_type, entity_id, entity_name, details, severity, created_at)
        VALUES ('admin', 1, 'System Administrator', 'admin@system.local', 'form_approved', 'admin', 'System Administrator approved Personal Information for John Doe', 'form_submission', '1', 'Personal Information', '{"student_name": "John Doe", "student_id": 2}', 'success', v_now - INTERVAL '2 days');

        -- Failed login attempt
        INSERT INTO activity_log (actor_type, actor_id, actor_name, actor_email, action_type, action_category, description, details, severity, ip_address, created_at)
        VALUES ('system', NULL, NULL, 'unknown@email.com', 'login_failed', 'authentication', 'Failed login attempt for unknown@email.com', '{"reason": "User not found"}', 'warning', '192.168.1.200', v_now - INTERVAL '1 day 12 hours');

        -- Instructor login
        INSERT INTO activity_log (actor_type, actor_id, actor_name, actor_email, action_type, action_category, description, severity, ip_address, created_at)
        VALUES ('instructor', 4, 'Bob Wilson', 'bob.wilson@school.edu', 'login_success', 'authentication', 'Bob Wilson logged in', 'success', '192.168.1.75', v_now - INTERVAL '1 day');

        -- Student completes all forms
        INSERT INTO activity_log (actor_type, actor_id, actor_name, actor_email, action_type, action_category, description, entity_type, entity_id, entity_name, severity, created_at)
        VALUES ('student', 2, 'John Doe', 'john.doe@email.com', 'form_submitted', 'forms', 'John Doe submitted Terms and Consent form', 'form_submission', '7', 'Terms and Consent', 'success', v_now - INTERVAL '18 hours');

        -- Admin revokes student access (warning severity)
        INSERT INTO activity_log (actor_type, actor_id, actor_name, actor_email, action_type, action_category, description, entity_type, entity_id, entity_name, severity, created_at)
        VALUES ('admin', 1, 'System Administrator', 'admin@system.local', 'access_revoked', 'admin', 'System Administrator revoked access for Test User', 'student', '99', 'Test User', 'warning', v_now - INTERVAL '12 hours');

        -- Recent student activity
        INSERT INTO activity_log (actor_type, actor_id, actor_name, actor_email, action_type, action_category, description, entity_type, entity_id, entity_name, severity, ip_address, created_at)
        VALUES ('student', 3, 'Jane Smith', 'jane.smith@email.com', 'form_submitted', 'forms', 'Jane Smith submitted Personal Information form', 'form_submission', '8', 'Personal Information', 'success', '192.168.1.51', v_now - INTERVAL '2 hours');

        -- Very recent activity
        INSERT INTO activity_log (actor_type, actor_id, actor_name, actor_email, action_type, action_category, description, severity, ip_address, created_at)
        VALUES ('admin', 1, 'System Administrator', 'admin@system.local', 'login_success', 'authentication', 'System Administrator logged in', 'success', '192.168.1.100', v_now - INTERVAL '30 minutes');

        INSERT INTO activity_log (actor_type, actor_id, actor_name, actor_email, action_type, action_category, description, entity_type, entity_name, severity, created_at)
        VALUES ('admin', 1, 'System Administrator', 'admin@system.local', 'settings_updated', 'admin', 'System Administrator updated institution settings', 'institution_settings', 'Institution Settings', 'info', v_now - INTERVAL '15 minutes');

        RAISE NOTICE 'Inserted sample activity log entries for demonstration';
    ELSE
        RAISE NOTICE 'Activity log already contains data, skipping seed data';
    END IF;
END $$;

-- =============================================================================
-- Comments for documentation
-- =============================================================================

COMMENT ON TABLE activity_log IS 'Unified audit trail capturing all user activities across the system';
COMMENT ON COLUMN activity_log.actor_type IS 'Type of user: student, instructor, admin, or system';
COMMENT ON COLUMN activity_log.actor_id IS 'Reference to app_user.id (NULL for system actions)';
COMMENT ON COLUMN activity_log.actor_name IS 'Denormalized name for display (captures name at time of action)';
COMMENT ON COLUMN activity_log.action_type IS 'Specific action: login_success, form_submitted, etc.';
COMMENT ON COLUMN activity_log.action_category IS 'Category: authentication, forms, profile, admin, system';
COMMENT ON COLUMN activity_log.description IS 'Human-readable description of what happened';
COMMENT ON COLUMN activity_log.entity_type IS 'Type of entity affected: student, form_submission, curriculum, etc.';
COMMENT ON COLUMN activity_log.entity_id IS 'ID of affected entity (string to support compound keys)';
COMMENT ON COLUMN activity_log.entity_name IS 'Denormalized name of affected entity';
COMMENT ON COLUMN activity_log.details IS 'Additional context stored as JSON';
COMMENT ON COLUMN activity_log.severity IS 'Severity level: info, success, warning, error';

-- =============================================================================
-- End of Migration
-- =============================================================================

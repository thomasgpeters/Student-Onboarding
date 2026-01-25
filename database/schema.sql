-- =============================================================================
-- Student Onboarding System - Consolidated Database Schema
-- =============================================================================
-- PostgreSQL Database Schema
-- Version: 2.0 (Consolidated from migrations 001-012)
-- Last Updated: January 2024
--
-- This schema includes all tables, indexes, triggers, and initial seed data
-- required for the Student Onboarding application.
--
-- Usage:
--   psql -U postgres -d student_onboarding -f schema.sql
--
-- For a complete installation, use: database/install.sql
-- =============================================================================

-- =====================================================
-- EXTENSIONS
-- =====================================================

-- Enable required PostgreSQL extensions (if not already enabled)
-- CREATE EXTENSION IF NOT EXISTS "uuid-ossp";

-- =====================================================
-- REFERENCE/LOOKUP TABLES
-- =====================================================

CREATE TABLE IF NOT EXISTS department (
    id SERIAL PRIMARY KEY,
    code VARCHAR(20) NOT NULL UNIQUE,
    name VARCHAR(200) NOT NULL,
    dean VARCHAR(200),
    contact_email VARCHAR(200),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS form_type (
    id SERIAL PRIMARY KEY,
    code VARCHAR(50) NOT NULL UNIQUE,
    name VARCHAR(200) NOT NULL,
    description TEXT,
    category VARCHAR(50),
    display_order INTEGER DEFAULT 0,
    is_required BOOLEAN DEFAULT FALSE,
    required_for_international BOOLEAN DEFAULT FALSE,
    required_for_transfer BOOLEAN DEFAULT FALSE,
    required_for_veteran BOOLEAN DEFAULT FALSE,
    required_for_financial_aid BOOLEAN DEFAULT FALSE,
    min_age INTEGER DEFAULT 0,
    max_age INTEGER DEFAULT 0,
    is_active BOOLEAN DEFAULT TRUE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- =====================================================
-- INSTITUTION SETTINGS TABLE
-- =====================================================
-- Centralized configuration for institution branding and modes
-- Added in migration 007, extended in 009

CREATE TABLE IF NOT EXISTS institution_settings (
    setting_key VARCHAR(100) PRIMARY KEY,
    setting_value TEXT,
    setting_type VARCHAR(50) DEFAULT 'string',
    category VARCHAR(50) DEFAULT 'general',
    display_name VARCHAR(200),
    description TEXT,
    is_required BOOLEAN DEFAULT FALSE,
    display_order INTEGER DEFAULT 0,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- =====================================================
-- APP_USER TABLE - Single Source of Truth for User Credentials
-- =====================================================
-- All users (students, instructors, admins) have their credentials here.
-- Profile-specific data is stored in student or admin_user tables.
-- Added in migration 014

CREATE TABLE IF NOT EXISTS app_user (
    id SERIAL PRIMARY KEY,
    email VARCHAR(200) NOT NULL UNIQUE,
    password_hash VARCHAR(500) NOT NULL,
    first_name VARCHAR(100),
    last_name VARCHAR(100),
    is_active BOOLEAN DEFAULT TRUE,
    login_enabled BOOLEAN DEFAULT TRUE,
    email_verified BOOLEAN DEFAULT FALSE,
    last_login_at TIMESTAMP,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- =====================================================
-- USER_ROLE TABLE - Role Assignments
-- =====================================================
-- A user can have multiple roles (e.g., both student and instructor)
-- Only administrators can assign roles
-- Added in migration 014

CREATE TABLE IF NOT EXISTS user_role (
    id SERIAL PRIMARY KEY,
    app_user_id INTEGER NOT NULL REFERENCES app_user(id) ON DELETE CASCADE,
    role VARCHAR(50) NOT NULL,
    assigned_by INTEGER REFERENCES app_user(id),
    assigned_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    is_active BOOLEAN DEFAULT TRUE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    -- Each user can only have each role once
    UNIQUE(app_user_id, role),
    -- Role constraint
    CONSTRAINT user_role_check
        CHECK (role IN ('student', 'instructor', 'admin', 'super_admin', 'super_user'))
);

-- =====================================================
-- CURRICULUM/PROGRAM TABLES
-- =====================================================
-- Extended to support both accredited and vocational programs
-- Includes CDL-specific fields from migration 004

CREATE TABLE IF NOT EXISTS curriculum (
    id SERIAL PRIMARY KEY,
    code VARCHAR(50) NOT NULL UNIQUE,
    name VARCHAR(200) NOT NULL,
    description TEXT,
    department_id INTEGER REFERENCES department(id),
    degree_type VARCHAR(50),
    credit_hours INTEGER DEFAULT 0,
    duration_semesters INTEGER DEFAULT 0,
    duration_interval VARCHAR(20) DEFAULT 'semester',
    is_active BOOLEAN DEFAULT TRUE,
    is_online BOOLEAN DEFAULT FALSE,
    -- Vocational/CDL support fields (migration 004)
    is_accredited BOOLEAN DEFAULT TRUE,
    program_type VARCHAR(50) DEFAULT 'academic',
    cdl_class VARCHAR(10),
    endorsements JSONB DEFAULT '[]',
    training_hours_classroom INTEGER,
    training_hours_range INTEGER,
    training_hours_road INTEGER,
    -- Endorsement program flag (base program vs add-on endorsement)
    is_endorsement BOOLEAN DEFAULT FALSE,
    -- Admin content field (migration 002)
    syllabus_content TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    -- Constraint for duration interval
    CONSTRAINT curriculum_duration_interval_check
        CHECK (duration_interval IN ('semester', 'month', 'week', 'day'))
);

CREATE TABLE IF NOT EXISTS curriculum_prerequisite (
    id SERIAL PRIMARY KEY,
    curriculum_id INTEGER NOT NULL REFERENCES curriculum(id) ON DELETE CASCADE,
    prerequisite_description VARCHAR(500) NOT NULL
);

CREATE TABLE IF NOT EXISTS curriculum_form_requirement (
    id SERIAL PRIMARY KEY,
    curriculum_id INTEGER NOT NULL REFERENCES curriculum(id) ON DELETE CASCADE,
    form_type_id INTEGER NOT NULL REFERENCES form_type(id),
    display_order INTEGER DEFAULT 0,
    UNIQUE(curriculum_id, form_type_id)
);

-- =====================================================
-- STUDENT TABLES
-- =====================================================
-- Extended with additional fields from migration 001

CREATE TABLE IF NOT EXISTS student (
    id SERIAL PRIMARY KEY,
    -- Reference to app_user for credentials (migration 014)
    app_user_id INTEGER REFERENCES app_user(id),
    -- Legacy columns kept for backward compatibility
    email VARCHAR(200) NOT NULL UNIQUE,
    password_hash VARCHAR(500),
    first_name VARCHAR(100),
    last_name VARCHAR(100),
    middle_name VARCHAR(100),
    preferred_name VARCHAR(100),
    preferred_pronouns VARCHAR(50),
    date_of_birth DATE,
    gender VARCHAR(20),
    phone_number VARCHAR(30),
    alternate_phone VARCHAR(30),
    -- Address fields (legacy, use student_address for multiple addresses)
    address_line1 VARCHAR(200),
    address_line2 VARCHAR(200),
    city VARCHAR(100),
    state VARCHAR(100),
    zip_code VARCHAR(20),
    -- Identity fields (migration 001)
    ssn VARCHAR(20),
    citizenship_status VARCHAR(50),
    -- Program enrollment
    curriculum_id INTEGER REFERENCES curriculum(id),
    student_type VARCHAR(50),
    enrollment_date DATE,
    expected_graduation DATE,
    -- Status flags
    is_international BOOLEAN DEFAULT FALSE,
    is_transfer_student BOOLEAN DEFAULT FALSE,
    is_veteran BOOLEAN DEFAULT FALSE,
    requires_financial_aid BOOLEAN DEFAULT FALSE,
    -- International student fields
    citizenship_country VARCHAR(100),
    visa_type VARCHAR(50),
    -- Intake status
    intake_status VARCHAR(50) DEFAULT 'in_progress',
    status VARCHAR(50) DEFAULT 'pending',
    completed_forms TEXT,
    -- Login control (migration 002)
    login_enabled BOOLEAN DEFAULT TRUE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Student addresses (migration 001)
-- Supports multiple address types: permanent, mailing, billing
CREATE TABLE IF NOT EXISTS student_address (
    id SERIAL PRIMARY KEY,
    student_id INTEGER NOT NULL REFERENCES student(id) ON DELETE CASCADE,
    address_type VARCHAR(50) NOT NULL,
    street1 VARCHAR(200),
    street2 VARCHAR(200),
    city VARCHAR(100),
    state VARCHAR(100),
    postal_code VARCHAR(20),
    country VARCHAR(100),
    is_primary BOOLEAN DEFAULT FALSE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- =====================================================
-- STUDENT ENDORSEMENT ENROLLMENT TABLE
-- =====================================================
-- Junction table for students to enroll in multiple endorsement programs
-- The student's primary program is stored in student.curriculum_id
-- Additional endorsements are stored here

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
-- ADMIN PORTAL TABLES (migration 002)
-- =====================================================

CREATE TABLE IF NOT EXISTS admin_user (
    id SERIAL PRIMARY KEY,
    -- Reference to app_user for credentials (migration 014)
    app_user_id INTEGER REFERENCES app_user(id),
    -- Legacy columns kept for backward compatibility
    email VARCHAR(200) NOT NULL UNIQUE,
    password_hash VARCHAR(500) NOT NULL,
    first_name VARCHAR(100),
    last_name VARCHAR(100),
    role VARCHAR(50) NOT NULL DEFAULT 'staff',
    department_id INTEGER REFERENCES department(id),
    is_active BOOLEAN DEFAULT TRUE,
    last_login_at TIMESTAMP,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    -- Role constraint
    CONSTRAINT admin_user_role_check
        CHECK (role IN ('staff', 'instructor', 'admin', 'super_admin', 'super_user'))
);

CREATE TABLE IF NOT EXISTS admin_program_assignment (
    id SERIAL PRIMARY KEY,
    admin_user_id INTEGER NOT NULL REFERENCES admin_user(id) ON DELETE CASCADE,
    curriculum_id INTEGER NOT NULL REFERENCES curriculum(id) ON DELETE CASCADE,
    can_view BOOLEAN DEFAULT TRUE,
    can_edit BOOLEAN DEFAULT FALSE,
    can_approve BOOLEAN DEFAULT FALSE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    UNIQUE(admin_user_id, curriculum_id)
);

CREATE TABLE IF NOT EXISTS admin_audit_log (
    id SERIAL PRIMARY KEY,
    admin_user_id INTEGER REFERENCES admin_user(id),
    action VARCHAR(100) NOT NULL,
    entity_type VARCHAR(100),
    entity_id INTEGER,
    old_value TEXT,
    new_value TEXT,
    ip_address VARCHAR(50),
    user_agent VARCHAR(500),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- =====================================================
-- FORM SUBMISSION TABLES
-- =====================================================

CREATE TABLE IF NOT EXISTS form_submission (
    id SERIAL PRIMARY KEY,
    student_id INTEGER NOT NULL REFERENCES student(id) ON DELETE CASCADE,
    form_type_id INTEGER REFERENCES form_type(id),
    form_type VARCHAR(50),
    session_id VARCHAR(100),
    status VARCHAR(50) DEFAULT 'draft',
    submitted_at TIMESTAMP,
    approved_at TIMESTAMP,
    approved_by VARCHAR(200),
    rejection_reason TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS form_field (
    id SERIAL PRIMARY KEY,
    form_submission_id INTEGER NOT NULL REFERENCES form_submission(id) ON DELETE CASCADE,
    field_name VARCHAR(100) NOT NULL,
    field_type VARCHAR(50) DEFAULT 'string',
    string_value TEXT,
    int_value INTEGER,
    double_value DOUBLE PRECISION,
    bool_value BOOLEAN,
    array_value TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- =====================================================
-- EMERGENCY CONTACT TABLE (migration 011 - compound key)
-- =====================================================
-- Uses compound primary key: (student_id, contact_relationship, phone)
-- This prevents duplicate entries for the same contact

CREATE TABLE IF NOT EXISTS emergency_contact (
    student_id INTEGER NOT NULL REFERENCES student(id) ON DELETE CASCADE,
    contact_relationship VARCHAR(50) NOT NULL,
    phone VARCHAR(30) NOT NULL,
    first_name VARCHAR(100) NOT NULL,
    last_name VARCHAR(100) NOT NULL,
    alternate_phone VARCHAR(30),
    email VARCHAR(200),
    street1 VARCHAR(200),
    street2 VARCHAR(200),
    city VARCHAR(100),
    state VARCHAR(100),
    postal_code VARCHAR(20),
    country VARCHAR(100),
    is_primary BOOLEAN DEFAULT FALSE,
    priority INTEGER DEFAULT 1,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    -- Compound primary key
    PRIMARY KEY (student_id, contact_relationship, phone)
);

-- =====================================================
-- MEDICAL INFORMATION TABLE
-- =====================================================

CREATE TABLE IF NOT EXISTS medical_info (
    id SERIAL PRIMARY KEY,
    student_id INTEGER NOT NULL UNIQUE REFERENCES student(id) ON DELETE CASCADE,
    blood_type VARCHAR(10),
    has_allergies BOOLEAN DEFAULT FALSE,
    allergies TEXT,
    has_medications BOOLEAN DEFAULT FALSE,
    medications TEXT,
    has_chronic_conditions BOOLEAN DEFAULT FALSE,
    chronic_conditions TEXT,
    has_disabilities BOOLEAN DEFAULT FALSE,
    disabilities TEXT,
    requires_accommodations BOOLEAN DEFAULT FALSE,
    accommodations_needed TEXT,
    insurance_provider VARCHAR(200),
    insurance_policy_number VARCHAR(100),
    insurance_group_number VARCHAR(100),
    insurance_phone VARCHAR(30),
    primary_physician VARCHAR(200),
    physician_phone VARCHAR(30),
    immunizations_up_to_date BOOLEAN DEFAULT FALSE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- =====================================================
-- ACADEMIC HISTORY TABLE (migration 010 - compound key)
-- =====================================================
-- Uses compound primary key: (student_id, institution_name, institution_type)
-- This prevents duplicate entries for the same institution

CREATE TABLE IF NOT EXISTS academic_history (
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

CREATE TABLE IF NOT EXISTS test_score (
    id SERIAL PRIMARY KEY,
    student_id INTEGER NOT NULL REFERENCES student(id) ON DELETE CASCADE,
    test_type VARCHAR(50) NOT NULL,
    test_date DATE,
    total_score DOUBLE PRECISION,
    section_scores TEXT,
    official_score_received BOOLEAN DEFAULT FALSE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- =====================================================
-- FINANCIAL AID TABLE (migration 012 - extended fields)
-- =====================================================

CREATE TABLE IF NOT EXISTS financial_aid (
    id SERIAL PRIMARY KEY,
    student_id INTEGER NOT NULL UNIQUE REFERENCES student(id) ON DELETE CASCADE,
    -- Basic FAFSA info
    fafsa_completed BOOLEAN DEFAULT FALSE,
    fafsa_year VARCHAR(20),
    efc DOUBLE PRECISION,
    -- Aid application status (migration 012)
    applying_for_aid BOOLEAN DEFAULT TRUE,
    aid_types TEXT,
    -- Employment info
    employment_status VARCHAR(50),
    employer_name VARCHAR(200),
    annual_income DOUBLE PRECISION,
    -- Household info
    household_size INTEGER,
    dependents_count INTEGER,
    household_income_range VARCHAR(100),
    is_independent BOOLEAN DEFAULT FALSE,
    -- Loan info
    has_outstanding_loans BOOLEAN DEFAULT FALSE,
    outstanding_loan_amount DOUBLE PRECISION,
    requested_aid_amount DOUBLE PRECISION,
    -- Interest flags (migration 012)
    veteran_benefits BOOLEAN DEFAULT FALSE,
    scholarship_interest BOOLEAN DEFAULT FALSE,
    work_study_interest BOOLEAN DEFAULT FALSE,
    loan_interest BOOLEAN DEFAULT FALSE,
    -- Additional info
    scholarship_applications TEXT,
    special_circumstances TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- =====================================================
-- DOCUMENT UPLOAD TABLE
-- =====================================================

CREATE TABLE IF NOT EXISTS document (
    id SERIAL PRIMARY KEY,
    student_id INTEGER NOT NULL REFERENCES student(id) ON DELETE CASCADE,
    document_type VARCHAR(50) NOT NULL,
    file_name VARCHAR(500) NOT NULL,
    file_size INTEGER,
    mime_type VARCHAR(100),
    storage_path VARCHAR(1000),
    status VARCHAR(50) DEFAULT 'pending',
    verified_at TIMESTAMP,
    verified_by VARCHAR(200),
    notes TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- =====================================================
-- CONSENT/AGREEMENT TABLE
-- =====================================================

CREATE TABLE IF NOT EXISTS consent (
    id SERIAL PRIMARY KEY,
    student_id INTEGER NOT NULL REFERENCES student(id) ON DELETE CASCADE,
    consent_type VARCHAR(100) NOT NULL,
    consent_version VARCHAR(50),
    is_accepted BOOLEAN DEFAULT FALSE,
    accepted_at TIMESTAMP,
    ip_address VARCHAR(50),
    electronic_signature VARCHAR(500),
    signature_date DATE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- =====================================================
-- AUDIT/SESSION TABLES
-- =====================================================

CREATE TABLE IF NOT EXISTS intake_session (
    id SERIAL PRIMARY KEY,
    student_id INTEGER NOT NULL REFERENCES student(id) ON DELETE CASCADE,
    session_token VARCHAR(500) UNIQUE,
    started_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    last_activity_at TIMESTAMP,
    completed_at TIMESTAMP,
    ip_address VARCHAR(50),
    user_agent VARCHAR(500)
);

CREATE TABLE IF NOT EXISTS audit_log (
    id SERIAL PRIMARY KEY,
    student_id INTEGER REFERENCES student(id),
    session_id INTEGER REFERENCES intake_session(id),
    action VARCHAR(100) NOT NULL,
    entity_type VARCHAR(100),
    entity_id INTEGER,
    old_value TEXT,
    new_value TEXT,
    ip_address VARCHAR(50),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- =====================================================
-- INDEXES FOR PERFORMANCE
-- =====================================================

-- AppUser indexes (migration 014)
CREATE INDEX IF NOT EXISTS idx_app_user_email ON app_user(email);
CREATE INDEX IF NOT EXISTS idx_app_user_active ON app_user(is_active);
CREATE INDEX IF NOT EXISTS idx_user_role_user ON user_role(app_user_id);
CREATE INDEX IF NOT EXISTS idx_user_role_role ON user_role(role);

-- Student indexes
CREATE INDEX IF NOT EXISTS idx_student_email ON student(email);
CREATE INDEX IF NOT EXISTS idx_student_app_user ON student(app_user_id);
CREATE INDEX IF NOT EXISTS idx_student_curriculum ON student(curriculum_id);
CREATE INDEX IF NOT EXISTS idx_student_status ON student(intake_status);
CREATE INDEX IF NOT EXISTS idx_student_name ON student(last_name, first_name);

-- Curriculum indexes
CREATE INDEX IF NOT EXISTS idx_curriculum_department ON curriculum(department_id);
CREATE INDEX IF NOT EXISTS idx_curriculum_program_type ON curriculum(program_type);
CREATE INDEX IF NOT EXISTS idx_curriculum_is_accredited ON curriculum(is_accredited);
CREATE INDEX IF NOT EXISTS idx_curriculum_cdl_class ON curriculum(cdl_class);
CREATE INDEX IF NOT EXISTS idx_curriculum_active ON curriculum(is_active);

-- Form indexes
CREATE INDEX IF NOT EXISTS idx_form_submission_student ON form_submission(student_id);
CREATE INDEX IF NOT EXISTS idx_form_submission_type ON form_submission(form_type_id);
CREATE INDEX IF NOT EXISTS idx_form_submission_form_type ON form_submission(form_type);
CREATE INDEX IF NOT EXISTS idx_form_submission_status ON form_submission(status);
CREATE INDEX IF NOT EXISTS idx_form_field_submission ON form_field(form_submission_id);

-- Related entity indexes
CREATE INDEX IF NOT EXISTS idx_emergency_contact_student ON emergency_contact(student_id);
CREATE INDEX IF NOT EXISTS idx_academic_history_student ON academic_history(student_id);
CREATE INDEX IF NOT EXISTS idx_document_student ON document(student_id);
CREATE INDEX IF NOT EXISTS idx_consent_student ON consent(student_id);

-- Endorsement enrollment indexes
CREATE INDEX IF NOT EXISTS idx_student_endorsement_student ON student_endorsement(student_id);
CREATE INDEX IF NOT EXISTS idx_student_endorsement_curriculum ON student_endorsement(curriculum_id);
CREATE INDEX IF NOT EXISTS idx_curriculum_is_endorsement ON curriculum(is_endorsement);

-- Admin user indexes (migration 014)
CREATE INDEX IF NOT EXISTS idx_admin_user_app_user ON admin_user(app_user_id);

-- Audit indexes
CREATE INDEX IF NOT EXISTS idx_audit_student ON audit_log(student_id);
CREATE INDEX IF NOT EXISTS idx_admin_audit_admin ON admin_audit_log(admin_user_id);
CREATE INDEX IF NOT EXISTS idx_admin_audit_action ON admin_audit_log(action);

-- =====================================================
-- FUNCTION: Update timestamp trigger
-- =====================================================

CREATE OR REPLACE FUNCTION update_updated_at_column()
RETURNS TRIGGER AS $$
BEGIN
    NEW.updated_at = CURRENT_TIMESTAMP;
    RETURN NEW;
END;
$$ language 'plpgsql';

-- =====================================================
-- TRIGGERS: Auto-update updated_at column
-- =====================================================

DROP TRIGGER IF EXISTS update_department_updated_at ON department;
CREATE TRIGGER update_department_updated_at BEFORE UPDATE ON department FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

DROP TRIGGER IF EXISTS update_form_type_updated_at ON form_type;
CREATE TRIGGER update_form_type_updated_at BEFORE UPDATE ON form_type FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

DROP TRIGGER IF EXISTS update_institution_settings_updated_at ON institution_settings;
CREATE TRIGGER update_institution_settings_updated_at BEFORE UPDATE ON institution_settings FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

DROP TRIGGER IF EXISTS update_app_user_updated_at ON app_user;
CREATE TRIGGER update_app_user_updated_at BEFORE UPDATE ON app_user FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

DROP TRIGGER IF EXISTS update_user_role_updated_at ON user_role;
CREATE TRIGGER update_user_role_updated_at BEFORE UPDATE ON user_role FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

DROP TRIGGER IF EXISTS update_curriculum_updated_at ON curriculum;
CREATE TRIGGER update_curriculum_updated_at BEFORE UPDATE ON curriculum FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

DROP TRIGGER IF EXISTS update_student_updated_at ON student;
CREATE TRIGGER update_student_updated_at BEFORE UPDATE ON student FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

DROP TRIGGER IF EXISTS update_student_address_updated_at ON student_address;
CREATE TRIGGER update_student_address_updated_at BEFORE UPDATE ON student_address FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

DROP TRIGGER IF EXISTS update_admin_user_updated_at ON admin_user;
CREATE TRIGGER update_admin_user_updated_at BEFORE UPDATE ON admin_user FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

DROP TRIGGER IF EXISTS update_form_submission_updated_at ON form_submission;
CREATE TRIGGER update_form_submission_updated_at BEFORE UPDATE ON form_submission FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

DROP TRIGGER IF EXISTS update_form_field_updated_at ON form_field;
CREATE TRIGGER update_form_field_updated_at BEFORE UPDATE ON form_field FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

DROP TRIGGER IF EXISTS update_emergency_contact_updated_at ON emergency_contact;
CREATE TRIGGER update_emergency_contact_updated_at BEFORE UPDATE ON emergency_contact FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

DROP TRIGGER IF EXISTS update_medical_info_updated_at ON medical_info;
CREATE TRIGGER update_medical_info_updated_at BEFORE UPDATE ON medical_info FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

DROP TRIGGER IF EXISTS update_academic_history_updated_at ON academic_history;
CREATE TRIGGER update_academic_history_updated_at BEFORE UPDATE ON academic_history FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

DROP TRIGGER IF EXISTS update_test_score_updated_at ON test_score;
CREATE TRIGGER update_test_score_updated_at BEFORE UPDATE ON test_score FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

DROP TRIGGER IF EXISTS update_financial_aid_updated_at ON financial_aid;
CREATE TRIGGER update_financial_aid_updated_at BEFORE UPDATE ON financial_aid FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

DROP TRIGGER IF EXISTS update_document_updated_at ON document;
CREATE TRIGGER update_document_updated_at BEFORE UPDATE ON document FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

DROP TRIGGER IF EXISTS update_consent_updated_at ON consent;
CREATE TRIGGER update_consent_updated_at BEFORE UPDATE ON consent FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

DROP TRIGGER IF EXISTS update_student_endorsement_updated_at ON student_endorsement;
CREATE TRIGGER update_student_endorsement_updated_at BEFORE UPDATE ON student_endorsement FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

-- =====================================================
-- END OF SCHEMA
-- =====================================================
-- For seed data, run: database/install.sql
-- For curriculum switching, use scripts in: database/scripts/
-- =====================================================

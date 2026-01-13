-- Student Intake Forms Database Schema - PostgreSQL
-- For use with ApiLogicServer
-- snake_case naming convention (PostgreSQL standard)

-- =====================================================
-- REFERENCE/LOOKUP TABLES
-- =====================================================

CREATE TABLE department (
    id SERIAL PRIMARY KEY,
    code VARCHAR(20) NOT NULL UNIQUE,
    name VARCHAR(200) NOT NULL,
    dean VARCHAR(200),
    contact_email VARCHAR(200),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE form_type (
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
-- CURRICULUM/PROGRAM TABLES
-- =====================================================

CREATE TABLE curriculum (
    id SERIAL PRIMARY KEY,
    code VARCHAR(50) NOT NULL UNIQUE,
    name VARCHAR(200) NOT NULL,
    description TEXT,
    department_id INTEGER REFERENCES department(id),
    degree_type VARCHAR(50),
    credit_hours INTEGER DEFAULT 0,
    duration_semesters INTEGER DEFAULT 0,
    is_active BOOLEAN DEFAULT TRUE,
    is_online BOOLEAN DEFAULT FALSE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE curriculum_prerequisite (
    id SERIAL PRIMARY KEY,
    curriculum_id INTEGER NOT NULL REFERENCES curriculum(id),
    prerequisite_description VARCHAR(500) NOT NULL
);

CREATE TABLE curriculum_form_requirement (
    id SERIAL PRIMARY KEY,
    curriculum_id INTEGER NOT NULL REFERENCES curriculum(id),
    form_type_id INTEGER NOT NULL REFERENCES form_type(id),
    UNIQUE(curriculum_id, form_type_id)
);

-- =====================================================
-- STUDENT TABLES
-- =====================================================

CREATE TABLE student (
    id SERIAL PRIMARY KEY,
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
    address_line1 VARCHAR(200),
    address_line2 VARCHAR(200),
    city VARCHAR(100),
    state VARCHAR(100),
    zip_code VARCHAR(20),
    ssn VARCHAR(20),
    citizenship_status VARCHAR(50),
    curriculum_id INTEGER REFERENCES curriculum(id),
    student_type VARCHAR(50),
    enrollment_date DATE,
    expected_graduation DATE,
    is_international BOOLEAN DEFAULT FALSE,
    is_transfer_student BOOLEAN DEFAULT FALSE,
    is_veteran BOOLEAN DEFAULT FALSE,
    requires_financial_aid BOOLEAN DEFAULT FALSE,
    citizenship_country VARCHAR(100),
    visa_type VARCHAR(50),
    intake_status VARCHAR(50) DEFAULT 'in_progress',
    completed_forms TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE student_address (
    id SERIAL PRIMARY KEY,
    student_id INTEGER NOT NULL REFERENCES student(id),
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
-- FORM SUBMISSION TABLES
-- =====================================================

CREATE TABLE form_submission (
    id SERIAL PRIMARY KEY,
    student_id INTEGER NOT NULL REFERENCES student(id),
    form_type_id INTEGER NOT NULL REFERENCES form_type(id),
    session_id VARCHAR(100),
    status VARCHAR(50) DEFAULT 'draft',
    submitted_at TIMESTAMP,
    approved_at TIMESTAMP,
    approved_by VARCHAR(200),
    rejection_reason TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE form_field (
    id SERIAL PRIMARY KEY,
    form_submission_id INTEGER NOT NULL REFERENCES form_submission(id),
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
-- EMERGENCY CONTACT TABLE
-- =====================================================

CREATE TABLE emergency_contact (
    id SERIAL PRIMARY KEY,
    student_id INTEGER NOT NULL REFERENCES student(id),
    first_name VARCHAR(100) NOT NULL,
    last_name VARCHAR(100) NOT NULL,
    contact_relationship VARCHAR(50),
    phone VARCHAR(30) NOT NULL,
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
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- =====================================================
-- MEDICAL INFORMATION TABLE
-- =====================================================

CREATE TABLE medical_info (
    id SERIAL PRIMARY KEY,
    student_id INTEGER NOT NULL UNIQUE REFERENCES student(id),
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
-- ACADEMIC HISTORY TABLES
-- =====================================================

CREATE TABLE academic_history (
    id SERIAL PRIMARY KEY,
    student_id INTEGER NOT NULL REFERENCES student(id),
    institution_name VARCHAR(200) NOT NULL,
    institution_type VARCHAR(50),
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
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE test_score (
    id SERIAL PRIMARY KEY,
    student_id INTEGER NOT NULL REFERENCES student(id),
    test_type VARCHAR(50) NOT NULL,
    test_date DATE,
    total_score DOUBLE PRECISION,
    section_scores TEXT,
    official_score_received BOOLEAN DEFAULT FALSE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- =====================================================
-- FINANCIAL AID TABLE
-- =====================================================

CREATE TABLE financial_aid (
    id SERIAL PRIMARY KEY,
    student_id INTEGER NOT NULL UNIQUE REFERENCES student(id),
    fafsa_completed BOOLEAN DEFAULT FALSE,
    fafsa_year VARCHAR(20),
    efc DOUBLE PRECISION,
    aid_types TEXT,
    employment_status VARCHAR(50),
    employer_name VARCHAR(200),
    annual_income DOUBLE PRECISION,
    household_size INTEGER,
    dependents_count INTEGER,
    is_independent BOOLEAN DEFAULT FALSE,
    has_outstanding_loans BOOLEAN DEFAULT FALSE,
    outstanding_loan_amount DOUBLE PRECISION,
    requested_aid_amount DOUBLE PRECISION,
    scholarship_applications TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- =====================================================
-- DOCUMENT UPLOAD TABLE
-- =====================================================

CREATE TABLE document (
    id SERIAL PRIMARY KEY,
    student_id INTEGER NOT NULL REFERENCES student(id),
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

CREATE TABLE consent (
    id SERIAL PRIMARY KEY,
    student_id INTEGER NOT NULL REFERENCES student(id),
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

CREATE TABLE intake_session (
    id SERIAL PRIMARY KEY,
    student_id INTEGER NOT NULL REFERENCES student(id),
    session_token VARCHAR(500) UNIQUE,
    started_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    last_activity_at TIMESTAMP,
    completed_at TIMESTAMP,
    ip_address VARCHAR(50),
    user_agent VARCHAR(500)
);

CREATE TABLE audit_log (
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

CREATE INDEX idx_student_email ON student(email);
CREATE INDEX idx_student_curriculum ON student(curriculum_id);
CREATE INDEX idx_student_status ON student(intake_status);
CREATE INDEX idx_curriculum_department ON curriculum(department_id);
CREATE INDEX idx_form_submission_student ON form_submission(student_id);
CREATE INDEX idx_form_submission_type ON form_submission(form_type_id);
CREATE INDEX idx_form_field_submission ON form_field(form_submission_id);
CREATE INDEX idx_emergency_contact_student ON emergency_contact(student_id);
CREATE INDEX idx_academic_history_student ON academic_history(student_id);
CREATE INDEX idx_document_student ON document(student_id);
CREATE INDEX idx_consent_student ON consent(student_id);
CREATE INDEX idx_audit_student ON audit_log(student_id);

-- =====================================================
-- SEED DATA - Form Types
-- =====================================================

INSERT INTO form_type (code, name, description, category, display_order, is_required, required_for_international, required_for_transfer, required_for_veteran, required_for_financial_aid) VALUES
('personal_info', 'Personal Information', 'Basic personal details and contact information', 'core', 1, TRUE, FALSE, FALSE, FALSE, FALSE),
('emergency_contact', 'Emergency Contacts', 'Emergency contact information', 'core', 2, TRUE, FALSE, FALSE, FALSE, FALSE),
('medical_info', 'Medical Information', 'Health and medical information', 'health', 3, FALSE, FALSE, FALSE, FALSE, FALSE),
('academic_history', 'Academic History', 'Educational background and transcripts', 'academic', 4, TRUE, FALSE, TRUE, FALSE, FALSE),
('financial_aid', 'Financial Aid', 'Financial aid application information', 'financial', 5, FALSE, FALSE, FALSE, FALSE, TRUE),
('documents', 'Document Upload', 'Required document submissions', 'documents', 6, FALSE, TRUE, TRUE, FALSE, FALSE),
('consent', 'Terms and Consent', 'Agreements and electronic signature', 'legal', 7, TRUE, FALSE, FALSE, FALSE, FALSE);

-- =====================================================
-- SEED DATA - Departments
-- =====================================================

INSERT INTO department (code, name, dean, contact_email) VALUES
('CS', 'Computer Science', 'Dr. Jane Smith', 'cs@university.edu'),
('BUS', 'Business Administration', 'Dr. John Davis', 'business@university.edu'),
('ENG', 'Engineering', 'Dr. Maria Garcia', 'engineering@university.edu'),
('SCI', 'Natural Sciences', 'Dr. Robert Wilson', 'sciences@university.edu'),
('ART', 'Arts and Humanities', 'Dr. Sarah Johnson', 'arts@university.edu');

-- =====================================================
-- SEED DATA - Sample Curriculums
-- =====================================================

INSERT INTO curriculum (code, name, description, department_id, degree_type, credit_hours, duration_semesters, is_active, is_online) VALUES
('CS-BS', 'Computer Science', 'Bachelor of Science in Computer Science', 1, 'bachelor', 120, 8, TRUE, FALSE),
('CS-MS', 'Computer Science', 'Master of Science in Computer Science', 1, 'master', 36, 4, TRUE, FALSE),
('BUS-MBA', 'Business Administration', 'Master of Business Administration', 2, 'master', 48, 4, TRUE, TRUE),
('BUS-BBA', 'Business Administration', 'Bachelor of Business Administration', 2, 'bachelor', 120, 8, TRUE, FALSE),
('ENG-BSME', 'Mechanical Engineering', 'Bachelor of Science in Mechanical Engineering', 3, 'bachelor', 128, 8, TRUE, FALSE),
('ENG-BSEE', 'Electrical Engineering', 'Bachelor of Science in Electrical Engineering', 3, 'bachelor', 128, 8, TRUE, FALSE),
('SCI-BS-BIO', 'Biology', 'Bachelor of Science in Biology', 4, 'bachelor', 120, 8, TRUE, FALSE),
('ART-BA-ENG', 'English Literature', 'Bachelor of Arts in English Literature', 5, 'bachelor', 120, 8, TRUE, FALSE);

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

CREATE TRIGGER update_department_updated_at BEFORE UPDATE ON department FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();
CREATE TRIGGER update_form_type_updated_at BEFORE UPDATE ON form_type FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();
CREATE TRIGGER update_curriculum_updated_at BEFORE UPDATE ON curriculum FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();
CREATE TRIGGER update_student_updated_at BEFORE UPDATE ON student FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();
CREATE TRIGGER update_student_address_updated_at BEFORE UPDATE ON student_address FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();
CREATE TRIGGER update_form_submission_updated_at BEFORE UPDATE ON form_submission FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();
CREATE TRIGGER update_form_field_updated_at BEFORE UPDATE ON form_field FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();
CREATE TRIGGER update_emergency_contact_updated_at BEFORE UPDATE ON emergency_contact FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();
CREATE TRIGGER update_medical_info_updated_at BEFORE UPDATE ON medical_info FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();
CREATE TRIGGER update_academic_history_updated_at BEFORE UPDATE ON academic_history FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();
CREATE TRIGGER update_test_score_updated_at BEFORE UPDATE ON test_score FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();
CREATE TRIGGER update_financial_aid_updated_at BEFORE UPDATE ON financial_aid FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();
CREATE TRIGGER update_document_updated_at BEFORE UPDATE ON document FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();
CREATE TRIGGER update_consent_updated_at BEFORE UPDATE ON consent FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

-- Student Intake Forms Database Schema - PostgreSQL
-- For use with ApiLogicServer
-- CamelCase naming convention, no quoted identifiers

-- =====================================================
-- REFERENCE/LOOKUP TABLES
-- =====================================================

CREATE TABLE Department (
    Id SERIAL PRIMARY KEY,
    Code VARCHAR(20) NOT NULL UNIQUE,
    Name VARCHAR(200) NOT NULL,
    Dean VARCHAR(200),
    ContactEmail VARCHAR(200),
    CreatedAt TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    UpdatedAt TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE FormType (
    Id SERIAL PRIMARY KEY,
    Code VARCHAR(50) NOT NULL UNIQUE,
    Name VARCHAR(200) NOT NULL,
    Description TEXT,
    Category VARCHAR(50),
    DisplayOrder INTEGER DEFAULT 0,
    IsRequired BOOLEAN DEFAULT FALSE,
    RequiredForInternational BOOLEAN DEFAULT FALSE,
    RequiredForTransfer BOOLEAN DEFAULT FALSE,
    RequiredForVeteran BOOLEAN DEFAULT FALSE,
    RequiredForFinancialAid BOOLEAN DEFAULT FALSE,
    MinAge INTEGER DEFAULT 0,
    MaxAge INTEGER DEFAULT 0,
    IsActive BOOLEAN DEFAULT TRUE,
    CreatedAt TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    UpdatedAt TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- =====================================================
-- CURRICULUM/PROGRAM TABLES
-- =====================================================

CREATE TABLE Curriculum (
    Id SERIAL PRIMARY KEY,
    Code VARCHAR(50) NOT NULL UNIQUE,
    Name VARCHAR(200) NOT NULL,
    Description TEXT,
    DepartmentId INTEGER,
    DegreeType VARCHAR(50),
    CreditHours INTEGER DEFAULT 0,
    DurationSemesters INTEGER DEFAULT 0,
    IsActive BOOLEAN DEFAULT TRUE,
    IsOnline BOOLEAN DEFAULT FALSE,
    CreatedAt TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    UpdatedAt TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT fk_curriculum_department FOREIGN KEY (DepartmentId) REFERENCES Department(Id)
);

CREATE TABLE CurriculumPrerequisite (
    Id SERIAL PRIMARY KEY,
    CurriculumId INTEGER NOT NULL,
    PrerequisiteDescription VARCHAR(500) NOT NULL,
    CONSTRAINT fk_curriculumprereq_curriculum FOREIGN KEY (CurriculumId) REFERENCES Curriculum(Id)
);

CREATE TABLE CurriculumFormRequirement (
    Id SERIAL PRIMARY KEY,
    CurriculumId INTEGER NOT NULL,
    FormTypeId INTEGER NOT NULL,
    CONSTRAINT fk_currformreq_curriculum FOREIGN KEY (CurriculumId) REFERENCES Curriculum(Id),
    CONSTRAINT fk_currformreq_formtype FOREIGN KEY (FormTypeId) REFERENCES FormType(Id),
    CONSTRAINT uq_curriculum_formtype UNIQUE(CurriculumId, FormTypeId)
);

-- =====================================================
-- STUDENT TABLES
-- =====================================================

CREATE TABLE Student (
    Id SERIAL PRIMARY KEY,
    Email VARCHAR(200) NOT NULL UNIQUE,
    PasswordHash VARCHAR(500),
    FirstName VARCHAR(100),
    LastName VARCHAR(100),
    MiddleName VARCHAR(100),
    PreferredName VARCHAR(100),
    DateOfBirth DATE,
    Gender VARCHAR(20),
    Phone VARCHAR(30),
    AlternatePhone VARCHAR(30),
    CurriculumId INTEGER,
    StudentType VARCHAR(50),
    EnrollmentDate DATE,
    ExpectedGraduation DATE,
    IsInternational BOOLEAN DEFAULT FALSE,
    IsTransferStudent BOOLEAN DEFAULT FALSE,
    IsVeteran BOOLEAN DEFAULT FALSE,
    RequiresFinancialAid BOOLEAN DEFAULT FALSE,
    CitizenshipCountry VARCHAR(100),
    VisaType VARCHAR(50),
    IntakeStatus VARCHAR(50) DEFAULT 'in_progress',
    CreatedAt TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    UpdatedAt TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT fk_student_curriculum FOREIGN KEY (CurriculumId) REFERENCES Curriculum(Id)
);

CREATE TABLE StudentAddress (
    Id SERIAL PRIMARY KEY,
    StudentId INTEGER NOT NULL,
    AddressType VARCHAR(50) NOT NULL,
    Street1 VARCHAR(200),
    Street2 VARCHAR(200),
    City VARCHAR(100),
    State VARCHAR(100),
    PostalCode VARCHAR(20),
    Country VARCHAR(100),
    IsPrimary BOOLEAN DEFAULT FALSE,
    CreatedAt TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    UpdatedAt TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT fk_studentaddress_student FOREIGN KEY (StudentId) REFERENCES Student(Id)
);

-- =====================================================
-- FORM SUBMISSION TABLES
-- =====================================================

CREATE TABLE FormSubmission (
    Id SERIAL PRIMARY KEY,
    StudentId INTEGER NOT NULL,
    FormTypeId INTEGER NOT NULL,
    SessionId VARCHAR(100),
    Status VARCHAR(50) DEFAULT 'draft',
    SubmittedAt TIMESTAMP,
    ApprovedAt TIMESTAMP,
    ApprovedBy VARCHAR(200),
    RejectionReason TEXT,
    CreatedAt TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    UpdatedAt TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT fk_formsubmission_student FOREIGN KEY (StudentId) REFERENCES Student(Id),
    CONSTRAINT fk_formsubmission_formtype FOREIGN KEY (FormTypeId) REFERENCES FormType(Id)
);

CREATE TABLE FormField (
    Id SERIAL PRIMARY KEY,
    FormSubmissionId INTEGER NOT NULL,
    FieldName VARCHAR(100) NOT NULL,
    FieldType VARCHAR(50) DEFAULT 'string',
    StringValue TEXT,
    IntValue INTEGER,
    DoubleValue DOUBLE PRECISION,
    BoolValue BOOLEAN,
    ArrayValue TEXT,
    CreatedAt TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    UpdatedAt TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT fk_formfield_submission FOREIGN KEY (FormSubmissionId) REFERENCES FormSubmission(Id)
);

-- =====================================================
-- EMERGENCY CONTACT TABLE
-- =====================================================

CREATE TABLE EmergencyContact (
    Id SERIAL PRIMARY KEY,
    StudentId INTEGER NOT NULL,
    FirstName VARCHAR(100) NOT NULL,
    LastName VARCHAR(100) NOT NULL,
    ContactRelationship VARCHAR(50),
    Phone VARCHAR(30) NOT NULL,
    AlternatePhone VARCHAR(30),
    Email VARCHAR(200),
    Street1 VARCHAR(200),
    Street2 VARCHAR(200),
    City VARCHAR(100),
    State VARCHAR(100),
    PostalCode VARCHAR(20),
    Country VARCHAR(100),
    IsPrimary BOOLEAN DEFAULT FALSE,
    Priority INTEGER DEFAULT 1,
    CreatedAt TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    UpdatedAt TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT fk_emergencycontact_student FOREIGN KEY (StudentId) REFERENCES Student(Id)
);

-- =====================================================
-- MEDICAL INFORMATION TABLE
-- =====================================================

CREATE TABLE MedicalInfo (
    Id SERIAL PRIMARY KEY,
    StudentId INTEGER NOT NULL UNIQUE,
    BloodType VARCHAR(10),
    HasAllergies BOOLEAN DEFAULT FALSE,
    Allergies TEXT,
    HasMedications BOOLEAN DEFAULT FALSE,
    Medications TEXT,
    HasChronicConditions BOOLEAN DEFAULT FALSE,
    ChronicConditions TEXT,
    HasDisabilities BOOLEAN DEFAULT FALSE,
    Disabilities TEXT,
    RequiresAccommodations BOOLEAN DEFAULT FALSE,
    AccommodationsNeeded TEXT,
    InsuranceProvider VARCHAR(200),
    InsurancePolicyNumber VARCHAR(100),
    InsuranceGroupNumber VARCHAR(100),
    InsurancePhone VARCHAR(30),
    PrimaryPhysician VARCHAR(200),
    PhysicianPhone VARCHAR(30),
    ImmunizationsUpToDate BOOLEAN DEFAULT FALSE,
    CreatedAt TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    UpdatedAt TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT fk_medicalinfo_student FOREIGN KEY (StudentId) REFERENCES Student(Id)
);

-- =====================================================
-- ACADEMIC HISTORY TABLES
-- =====================================================

CREATE TABLE AcademicHistory (
    Id SERIAL PRIMARY KEY,
    StudentId INTEGER NOT NULL,
    InstitutionName VARCHAR(200) NOT NULL,
    InstitutionType VARCHAR(50),
    InstitutionCity VARCHAR(100),
    InstitutionState VARCHAR(100),
    InstitutionCountry VARCHAR(100),
    DegreeEarned VARCHAR(100),
    Major VARCHAR(200),
    Minor VARCHAR(200),
    GPA DOUBLE PRECISION,
    GPAScale DOUBLE PRECISION DEFAULT 4.0,
    StartDate DATE,
    EndDate DATE,
    GraduationDate DATE,
    IsCurrentlyAttending BOOLEAN DEFAULT FALSE,
    TranscriptReceived BOOLEAN DEFAULT FALSE,
    CreatedAt TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    UpdatedAt TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT fk_academichistory_student FOREIGN KEY (StudentId) REFERENCES Student(Id)
);

CREATE TABLE TestScore (
    Id SERIAL PRIMARY KEY,
    StudentId INTEGER NOT NULL,
    TestType VARCHAR(50) NOT NULL,
    TestDate DATE,
    TotalScore DOUBLE PRECISION,
    SectionScores TEXT,
    OfficialScoreReceived BOOLEAN DEFAULT FALSE,
    CreatedAt TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    UpdatedAt TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT fk_testscore_student FOREIGN KEY (StudentId) REFERENCES Student(Id)
);

-- =====================================================
-- FINANCIAL AID TABLE
-- =====================================================

CREATE TABLE FinancialAid (
    Id SERIAL PRIMARY KEY,
    StudentId INTEGER NOT NULL UNIQUE,
    FAFSACompleted BOOLEAN DEFAULT FALSE,
    FAFSAYear VARCHAR(20),
    EFC DOUBLE PRECISION,
    AidTypes TEXT,
    EmploymentStatus VARCHAR(50),
    EmployerName VARCHAR(200),
    AnnualIncome DOUBLE PRECISION,
    HouseholdSize INTEGER,
    DependentsCount INTEGER,
    IsIndependent BOOLEAN DEFAULT FALSE,
    HasOutstandingLoans BOOLEAN DEFAULT FALSE,
    OutstandingLoanAmount DOUBLE PRECISION,
    RequestedAidAmount DOUBLE PRECISION,
    ScholarshipApplications TEXT,
    CreatedAt TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    UpdatedAt TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT fk_financialaid_student FOREIGN KEY (StudentId) REFERENCES Student(Id)
);

-- =====================================================
-- DOCUMENT UPLOAD TABLE
-- =====================================================

CREATE TABLE Document (
    Id SERIAL PRIMARY KEY,
    StudentId INTEGER NOT NULL,
    DocumentType VARCHAR(50) NOT NULL,
    FileName VARCHAR(500) NOT NULL,
    FileSize INTEGER,
    MimeType VARCHAR(100),
    StoragePath VARCHAR(1000),
    Status VARCHAR(50) DEFAULT 'pending',
    VerifiedAt TIMESTAMP,
    VerifiedBy VARCHAR(200),
    Notes TEXT,
    CreatedAt TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    UpdatedAt TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT fk_document_student FOREIGN KEY (StudentId) REFERENCES Student(Id)
);

-- =====================================================
-- CONSENT/AGREEMENT TABLE
-- =====================================================

CREATE TABLE Consent (
    Id SERIAL PRIMARY KEY,
    StudentId INTEGER NOT NULL,
    ConsentType VARCHAR(100) NOT NULL,
    ConsentVersion VARCHAR(50),
    IsAccepted BOOLEAN DEFAULT FALSE,
    AcceptedAt TIMESTAMP,
    IPAddress VARCHAR(50),
    ElectronicSignature VARCHAR(500),
    SignatureDate DATE,
    CreatedAt TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    UpdatedAt TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT fk_consent_student FOREIGN KEY (StudentId) REFERENCES Student(Id)
);

-- =====================================================
-- AUDIT/SESSION TABLES
-- =====================================================

CREATE TABLE IntakeSession (
    Id SERIAL PRIMARY KEY,
    StudentId INTEGER NOT NULL,
    SessionToken VARCHAR(500) UNIQUE,
    StartedAt TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    LastActivityAt TIMESTAMP,
    CompletedAt TIMESTAMP,
    IPAddress VARCHAR(50),
    UserAgent VARCHAR(500),
    CONSTRAINT fk_intakesession_student FOREIGN KEY (StudentId) REFERENCES Student(Id)
);

CREATE TABLE AuditLog (
    Id SERIAL PRIMARY KEY,
    StudentId INTEGER,
    SessionId INTEGER,
    Action VARCHAR(100) NOT NULL,
    EntityType VARCHAR(100),
    EntityId INTEGER,
    OldValue TEXT,
    NewValue TEXT,
    IPAddress VARCHAR(50),
    CreatedAt TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT fk_auditlog_student FOREIGN KEY (StudentId) REFERENCES Student(Id),
    CONSTRAINT fk_auditlog_session FOREIGN KEY (SessionId) REFERENCES IntakeSession(Id)
);

-- =====================================================
-- INDEXES FOR PERFORMANCE
-- =====================================================

CREATE INDEX idx_student_email ON Student(Email);
CREATE INDEX idx_student_curriculum ON Student(CurriculumId);
CREATE INDEX idx_student_status ON Student(IntakeStatus);
CREATE INDEX idx_curriculum_department ON Curriculum(DepartmentId);
CREATE INDEX idx_form_submission_student ON FormSubmission(StudentId);
CREATE INDEX idx_form_submission_type ON FormSubmission(FormTypeId);
CREATE INDEX idx_form_field_submission ON FormField(FormSubmissionId);
CREATE INDEX idx_emergency_contact_student ON EmergencyContact(StudentId);
CREATE INDEX idx_academic_history_student ON AcademicHistory(StudentId);
CREATE INDEX idx_document_student ON Document(StudentId);
CREATE INDEX idx_consent_student ON Consent(StudentId);
CREATE INDEX idx_audit_student ON AuditLog(StudentId);

-- =====================================================
-- SEED DATA - Form Types
-- =====================================================

INSERT INTO FormType (Code, Name, Description, Category, DisplayOrder, IsRequired, RequiredForInternational, RequiredForTransfer, RequiredForVeteran, RequiredForFinancialAid) VALUES
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

INSERT INTO Department (Code, Name, Dean, ContactEmail) VALUES
('CS', 'Computer Science', 'Dr. Jane Smith', 'cs@university.edu'),
('BUS', 'Business Administration', 'Dr. John Davis', 'business@university.edu'),
('ENG', 'Engineering', 'Dr. Maria Garcia', 'engineering@university.edu'),
('SCI', 'Natural Sciences', 'Dr. Robert Wilson', 'sciences@university.edu'),
('ART', 'Arts and Humanities', 'Dr. Sarah Johnson', 'arts@university.edu');

-- =====================================================
-- SEED DATA - Sample Curriculums
-- =====================================================

INSERT INTO Curriculum (Code, Name, Description, DepartmentId, DegreeType, CreditHours, DurationSemesters, IsActive, IsOnline) VALUES
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
    NEW.UpdatedAt = CURRENT_TIMESTAMP;
    RETURN NEW;
END;
$$ language 'plpgsql';

-- =====================================================
-- TRIGGERS: Auto-update UpdatedAt column
-- =====================================================

CREATE TRIGGER update_department_updated_at BEFORE UPDATE ON Department FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();
CREATE TRIGGER update_formtype_updated_at BEFORE UPDATE ON FormType FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();
CREATE TRIGGER update_curriculum_updated_at BEFORE UPDATE ON Curriculum FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();
CREATE TRIGGER update_student_updated_at BEFORE UPDATE ON Student FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();
CREATE TRIGGER update_studentaddress_updated_at BEFORE UPDATE ON StudentAddress FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();
CREATE TRIGGER update_formsubmission_updated_at BEFORE UPDATE ON FormSubmission FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();
CREATE TRIGGER update_formfield_updated_at BEFORE UPDATE ON FormField FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();
CREATE TRIGGER update_emergencycontact_updated_at BEFORE UPDATE ON EmergencyContact FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();
CREATE TRIGGER update_medicalinfo_updated_at BEFORE UPDATE ON MedicalInfo FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();
CREATE TRIGGER update_academichistory_updated_at BEFORE UPDATE ON AcademicHistory FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();
CREATE TRIGGER update_testscore_updated_at BEFORE UPDATE ON TestScore FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();
CREATE TRIGGER update_financialaid_updated_at BEFORE UPDATE ON FinancialAid FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();
CREATE TRIGGER update_document_updated_at BEFORE UPDATE ON Document FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();
CREATE TRIGGER update_consent_updated_at BEFORE UPDATE ON Consent FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();

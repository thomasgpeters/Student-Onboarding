-- Student Intake Forms Database Schema
-- For use with ApiLogicServer
-- CamelCase naming convention, no quoted identifiers

-- =====================================================
-- REFERENCE/LOOKUP TABLES
-- =====================================================

CREATE TABLE Department (
    Id INTEGER PRIMARY KEY AUTOINCREMENT,
    Code VARCHAR(20) NOT NULL UNIQUE,
    Name VARCHAR(200) NOT NULL,
    Dean VARCHAR(200),
    ContactEmail VARCHAR(200),
    CreatedAt DATETIME DEFAULT CURRENT_TIMESTAMP,
    UpdatedAt DATETIME DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE FormType (
    Id INTEGER PRIMARY KEY AUTOINCREMENT,
    Code VARCHAR(50) NOT NULL UNIQUE,
    Name VARCHAR(200) NOT NULL,
    Description TEXT,
    Category VARCHAR(50),
    DisplayOrder INTEGER DEFAULT 0,
    IsRequired BOOLEAN DEFAULT 0,
    RequiredForInternational BOOLEAN DEFAULT 0,
    RequiredForTransfer BOOLEAN DEFAULT 0,
    RequiredForVeteran BOOLEAN DEFAULT 0,
    RequiredForFinancialAid BOOLEAN DEFAULT 0,
    MinAge INTEGER DEFAULT 0,
    MaxAge INTEGER DEFAULT 0,
    IsActive BOOLEAN DEFAULT 1,
    CreatedAt DATETIME DEFAULT CURRENT_TIMESTAMP,
    UpdatedAt DATETIME DEFAULT CURRENT_TIMESTAMP
);

-- =====================================================
-- CURRICULUM/PROGRAM TABLES
-- =====================================================

CREATE TABLE Curriculum (
    Id INTEGER PRIMARY KEY AUTOINCREMENT,
    Code VARCHAR(50) NOT NULL UNIQUE,
    Name VARCHAR(200) NOT NULL,
    Description TEXT,
    DepartmentId INTEGER,
    DegreeType VARCHAR(50),  -- bachelor, master, doctoral, certificate, associate
    CreditHours INTEGER DEFAULT 0,
    DurationSemesters INTEGER DEFAULT 0,
    IsActive BOOLEAN DEFAULT 1,
    IsOnline BOOLEAN DEFAULT 0,
    CreatedAt DATETIME DEFAULT CURRENT_TIMESTAMP,
    UpdatedAt DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (DepartmentId) REFERENCES Department(Id)
);

CREATE TABLE CurriculumPrerequisite (
    Id INTEGER PRIMARY KEY AUTOINCREMENT,
    CurriculumId INTEGER NOT NULL,
    PrerequisiteDescription VARCHAR(500) NOT NULL,
    FOREIGN KEY (CurriculumId) REFERENCES Curriculum(Id)
);

CREATE TABLE CurriculumFormRequirement (
    Id INTEGER PRIMARY KEY AUTOINCREMENT,
    CurriculumId INTEGER NOT NULL,
    FormTypeId INTEGER NOT NULL,
    FOREIGN KEY (CurriculumId) REFERENCES Curriculum(Id),
    FOREIGN KEY (FormTypeId) REFERENCES FormType(Id),
    UNIQUE(CurriculumId, FormTypeId)
);

-- =====================================================
-- STUDENT TABLES
-- =====================================================

CREATE TABLE Student (
    Id INTEGER PRIMARY KEY AUTOINCREMENT,
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
    StudentType VARCHAR(50),  -- undergraduate, graduate, doctoral, certificate
    EnrollmentDate DATE,
    ExpectedGraduation DATE,
    IsInternational BOOLEAN DEFAULT 0,
    IsTransferStudent BOOLEAN DEFAULT 0,
    IsVeteran BOOLEAN DEFAULT 0,
    RequiresFinancialAid BOOLEAN DEFAULT 0,
    CitizenshipCountry VARCHAR(100),
    VisaType VARCHAR(50),
    IntakeStatus VARCHAR(50) DEFAULT 'in_progress',  -- in_progress, submitted, approved, rejected
    CreatedAt DATETIME DEFAULT CURRENT_TIMESTAMP,
    UpdatedAt DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (CurriculumId) REFERENCES Curriculum(Id)
);

CREATE TABLE StudentAddress (
    Id INTEGER PRIMARY KEY AUTOINCREMENT,
    StudentId INTEGER NOT NULL,
    AddressType VARCHAR(50) NOT NULL,  -- permanent, mailing, billing
    Street1 VARCHAR(200),
    Street2 VARCHAR(200),
    City VARCHAR(100),
    State VARCHAR(100),
    PostalCode VARCHAR(20),
    Country VARCHAR(100),
    IsPrimary BOOLEAN DEFAULT 0,
    CreatedAt DATETIME DEFAULT CURRENT_TIMESTAMP,
    UpdatedAt DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (StudentId) REFERENCES Student(Id)
);

-- =====================================================
-- FORM SUBMISSION TABLES
-- =====================================================

CREATE TABLE FormSubmission (
    Id INTEGER PRIMARY KEY AUTOINCREMENT,
    StudentId INTEGER NOT NULL,
    FormTypeId INTEGER NOT NULL,
    SessionId VARCHAR(100),
    Status VARCHAR(50) DEFAULT 'draft',  -- draft, submitted, approved, rejected
    SubmittedAt DATETIME,
    ApprovedAt DATETIME,
    ApprovedBy VARCHAR(200),
    RejectionReason TEXT,
    CreatedAt DATETIME DEFAULT CURRENT_TIMESTAMP,
    UpdatedAt DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (StudentId) REFERENCES Student(Id),
    FOREIGN KEY (FormTypeId) REFERENCES FormType(Id)
);

CREATE TABLE FormField (
    Id INTEGER PRIMARY KEY AUTOINCREMENT,
    FormSubmissionId INTEGER NOT NULL,
    FieldName VARCHAR(100) NOT NULL,
    FieldType VARCHAR(50) DEFAULT 'string',  -- string, int, double, bool, array, file
    StringValue TEXT,
    IntValue INTEGER,
    DoubleValue REAL,
    BoolValue BOOLEAN,
    ArrayValue TEXT,  -- JSON array as string
    CreatedAt DATETIME DEFAULT CURRENT_TIMESTAMP,
    UpdatedAt DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (FormSubmissionId) REFERENCES FormSubmission(Id)
);

-- =====================================================
-- EMERGENCY CONTACT TABLE
-- =====================================================

CREATE TABLE EmergencyContact (
    Id INTEGER PRIMARY KEY AUTOINCREMENT,
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
    IsPrimary BOOLEAN DEFAULT 0,
    Priority INTEGER DEFAULT 1,
    CreatedAt DATETIME DEFAULT CURRENT_TIMESTAMP,
    UpdatedAt DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (StudentId) REFERENCES Student(Id)
);

-- =====================================================
-- MEDICAL INFORMATION TABLE
-- =====================================================

CREATE TABLE MedicalInfo (
    Id INTEGER PRIMARY KEY AUTOINCREMENT,
    StudentId INTEGER NOT NULL UNIQUE,
    BloodType VARCHAR(10),
    HasAllergies BOOLEAN DEFAULT 0,
    Allergies TEXT,
    HasMedications BOOLEAN DEFAULT 0,
    Medications TEXT,
    HasChronicConditions BOOLEAN DEFAULT 0,
    ChronicConditions TEXT,
    HasDisabilities BOOLEAN DEFAULT 0,
    Disabilities TEXT,
    RequiresAccommodations BOOLEAN DEFAULT 0,
    AccommodationsNeeded TEXT,
    InsuranceProvider VARCHAR(200),
    InsurancePolicyNumber VARCHAR(100),
    InsuranceGroupNumber VARCHAR(100),
    InsurancePhone VARCHAR(30),
    PrimaryPhysician VARCHAR(200),
    PhysicianPhone VARCHAR(30),
    ImmunizationsUpToDate BOOLEAN DEFAULT 0,
    CreatedAt DATETIME DEFAULT CURRENT_TIMESTAMP,
    UpdatedAt DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (StudentId) REFERENCES Student(Id)
);

-- =====================================================
-- ACADEMIC HISTORY TABLES
-- =====================================================

CREATE TABLE AcademicHistory (
    Id INTEGER PRIMARY KEY AUTOINCREMENT,
    StudentId INTEGER NOT NULL,
    InstitutionName VARCHAR(200) NOT NULL,
    InstitutionType VARCHAR(50),  -- high_school, community_college, university, trade_school
    InstitutionCity VARCHAR(100),
    InstitutionState VARCHAR(100),
    InstitutionCountry VARCHAR(100),
    DegreeEarned VARCHAR(100),
    Major VARCHAR(200),
    Minor VARCHAR(200),
    GPA REAL,
    GPAScale REAL DEFAULT 4.0,
    StartDate DATE,
    EndDate DATE,
    GraduationDate DATE,
    IsCurrentlyAttending BOOLEAN DEFAULT 0,
    TranscriptReceived BOOLEAN DEFAULT 0,
    CreatedAt DATETIME DEFAULT CURRENT_TIMESTAMP,
    UpdatedAt DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (StudentId) REFERENCES Student(Id)
);

CREATE TABLE TestScore (
    Id INTEGER PRIMARY KEY AUTOINCREMENT,
    StudentId INTEGER NOT NULL,
    TestType VARCHAR(50) NOT NULL,  -- SAT, ACT, GRE, GMAT, TOEFL, IELTS
    TestDate DATE,
    TotalScore REAL,
    SectionScores TEXT,  -- JSON object with section breakdowns
    OfficialScoreReceived BOOLEAN DEFAULT 0,
    CreatedAt DATETIME DEFAULT CURRENT_TIMESTAMP,
    UpdatedAt DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (StudentId) REFERENCES Student(Id)
);

-- =====================================================
-- FINANCIAL AID TABLE
-- =====================================================

CREATE TABLE FinancialAid (
    Id INTEGER PRIMARY KEY AUTOINCREMENT,
    StudentId INTEGER NOT NULL UNIQUE,
    FAFSACompleted BOOLEAN DEFAULT 0,
    FAFSAYear VARCHAR(20),
    EFC REAL,  -- Expected Family Contribution
    AidTypes TEXT,  -- JSON array: grants, loans, scholarships, work_study
    EmploymentStatus VARCHAR(50),
    EmployerName VARCHAR(200),
    AnnualIncome REAL,
    HouseholdSize INTEGER,
    DependentsCount INTEGER,
    IsIndependent BOOLEAN DEFAULT 0,
    HasOutstandingLoans BOOLEAN DEFAULT 0,
    OutstandingLoanAmount REAL,
    RequestedAidAmount REAL,
    ScholarshipApplications TEXT,  -- JSON array of scholarship IDs applied for
    CreatedAt DATETIME DEFAULT CURRENT_TIMESTAMP,
    UpdatedAt DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (StudentId) REFERENCES Student(Id)
);

-- =====================================================
-- DOCUMENT UPLOAD TABLE
-- =====================================================

CREATE TABLE Document (
    Id INTEGER PRIMARY KEY AUTOINCREMENT,
    StudentId INTEGER NOT NULL,
    DocumentType VARCHAR(50) NOT NULL,  -- transcript, id, visa, immunization, etc.
    FileName VARCHAR(500) NOT NULL,
    FileSize INTEGER,
    MimeType VARCHAR(100),
    StoragePath VARCHAR(1000),
    Status VARCHAR(50) DEFAULT 'pending',  -- pending, verified, rejected
    VerifiedAt DATETIME,
    VerifiedBy VARCHAR(200),
    Notes TEXT,
    CreatedAt DATETIME DEFAULT CURRENT_TIMESTAMP,
    UpdatedAt DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (StudentId) REFERENCES Student(Id)
);

-- =====================================================
-- CONSENT/AGREEMENT TABLE
-- =====================================================

CREATE TABLE Consent (
    Id INTEGER PRIMARY KEY AUTOINCREMENT,
    StudentId INTEGER NOT NULL,
    ConsentType VARCHAR(100) NOT NULL,  -- ferpa, terms_of_service, photo_release, etc.
    ConsentVersion VARCHAR(50),
    IsAccepted BOOLEAN DEFAULT 0,
    AcceptedAt DATETIME,
    IPAddress VARCHAR(50),
    ElectronicSignature VARCHAR(500),
    SignatureDate DATE,
    CreatedAt DATETIME DEFAULT CURRENT_TIMESTAMP,
    UpdatedAt DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (StudentId) REFERENCES Student(Id)
);

-- =====================================================
-- AUDIT/SESSION TABLES
-- =====================================================

CREATE TABLE IntakeSession (
    Id INTEGER PRIMARY KEY AUTOINCREMENT,
    StudentId INTEGER NOT NULL,
    SessionToken VARCHAR(500) UNIQUE,
    StartedAt DATETIME DEFAULT CURRENT_TIMESTAMP,
    LastActivityAt DATETIME,
    CompletedAt DATETIME,
    IPAddress VARCHAR(50),
    UserAgent VARCHAR(500),
    FOREIGN KEY (StudentId) REFERENCES Student(Id)
);

CREATE TABLE AuditLog (
    Id INTEGER PRIMARY KEY AUTOINCREMENT,
    StudentId INTEGER,
    SessionId INTEGER,
    Action VARCHAR(100) NOT NULL,
    EntityType VARCHAR(100),
    EntityId INTEGER,
    OldValue TEXT,
    NewValue TEXT,
    IPAddress VARCHAR(50),
    CreatedAt DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (StudentId) REFERENCES Student(Id),
    FOREIGN KEY (SessionId) REFERENCES IntakeSession(Id)
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
('personal_info', 'Personal Information', 'Basic personal details and contact information', 'core', 1, 1, 0, 0, 0, 0),
('emergency_contact', 'Emergency Contacts', 'Emergency contact information', 'core', 2, 1, 0, 0, 0, 0),
('medical_info', 'Medical Information', 'Health and medical information', 'health', 3, 0, 0, 0, 0, 0),
('academic_history', 'Academic History', 'Educational background and transcripts', 'academic', 4, 1, 0, 1, 0, 0),
('financial_aid', 'Financial Aid', 'Financial aid application information', 'financial', 5, 0, 0, 0, 0, 1),
('documents', 'Document Upload', 'Required document submissions', 'documents', 6, 0, 1, 1, 0, 0),
('consent', 'Terms and Consent', 'Agreements and electronic signature', 'legal', 7, 1, 0, 0, 0, 0);

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
('CS-BS', 'Computer Science', 'Bachelor of Science in Computer Science', 1, 'bachelor', 120, 8, 1, 0),
('CS-MS', 'Computer Science', 'Master of Science in Computer Science', 1, 'master', 36, 4, 1, 0),
('BUS-MBA', 'Business Administration', 'Master of Business Administration', 2, 'master', 48, 4, 1, 1),
('BUS-BBA', 'Business Administration', 'Bachelor of Business Administration', 2, 'bachelor', 120, 8, 1, 0),
('ENG-BSME', 'Mechanical Engineering', 'Bachelor of Science in Mechanical Engineering', 3, 'bachelor', 128, 8, 1, 0),
('ENG-BSEE', 'Electrical Engineering', 'Bachelor of Science in Electrical Engineering', 3, 'bachelor', 128, 8, 1, 0),
('SCI-BS-BIO', 'Biology', 'Bachelor of Science in Biology', 4, 'bachelor', 120, 8, 1, 0),
('ART-BA-ENG', 'English Literature', 'Bachelor of Arts in English Literature', 5, 'bachelor', 120, 8, 1, 0);

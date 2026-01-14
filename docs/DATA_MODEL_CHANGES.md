# Data Model Changes

This document tracks changes to the database schema and provides migration instructions for the Student Onboarding application.

## Overview

The application uses a PostgreSQL database with ApiLogicServer as the middleware layer. When schema changes are made, both the database and ApiLogicServer must be updated.

---

## Change Log

### Version 1.5.0 - AcademicHistory Model for Multiple Education Records

**Date:** January 2026
**Purpose:** Support multiple academic history records per student (high school, colleges, universities).

#### Architecture Change

Academic history is now stored in the `academic_history` table with an `institution_type` field. This allows:

1. **Multiple records per student** - High school, multiple colleges, universities
2. **Different institution types** - "High School", "College", "University", "Trade School"
3. **Independent record management** - Each record has its own primary key

#### `academic_history` Table Schema

| Column Name | Type | Description |
|-------------|------|-------------|
| `id` | SERIAL | Primary key |
| `student_id` | INTEGER | Foreign key to student table |
| `institution_name` | VARCHAR(200) | Name of the institution |
| `institution_type` | VARCHAR(50) | Type: 'High School', 'College', 'University' |
| `institution_city` | VARCHAR(100) | City |
| `institution_state` | VARCHAR(100) | State/Province |
| `institution_country` | VARCHAR(100) | Country |
| `degree_earned` | VARCHAR(100) | Degree type or 'GED', 'High School Diploma' |
| `major` | VARCHAR(200) | Major/Field of study |
| `minor` | VARCHAR(200) | Minor (optional) |
| `gpa` | DOUBLE | Grade point average |
| `gpa_scale` | DOUBLE | GPA scale (default 4.0) |
| `start_date` | DATE | Start date |
| `end_date` | DATE | End date |
| `graduation_date` | DATE | Graduation date |
| `is_currently_attending` | BOOLEAN | Currently attending flag |
| `transcript_received` | BOOLEAN | Transcript received flag |

#### New C++ Classes

- **`Models::AcademicHistory`** - Model class for academic history records
  - `src/models/AcademicHistory.h`
  - `src/models/AcademicHistory.cpp`

#### New API Methods in `FormSubmissionService`

- `getAcademicHistories(studentId)` - Get all academic records for a student
- `getAcademicHistoryByType(studentId, institutionType)` - Get record by institution type
- `createAcademicHistory(history)` - Create new record
- `updateAcademicHistory(history)` - Update existing record
- `deleteAcademicHistory(historyId)` - Delete record
- `saveAcademicHistory(history)` - Create or update (upsert by institution type)

#### Form Integration

The `AcademicHistoryForm` now:
- Loads existing records from API on form display
- Saves high school as record with `institution_type = "High School"`
- Saves college as record with `institution_type = "College"`
- Pre-fills form fields when returning to the form

---

### Version 1.4.0 - FinancialAid Form Field Mapping Fix

**Date:** January 2026
**Purpose:** Fix Financial Aid form submission by properly mapping form fields to database columns.

#### Issue

The Financial Aid form fields did not match the `financial_aid` table columns, causing submission failures.

#### Solution

Updated `submitFinancialAid()` in `FormSubmissionService` to:
1. Map form fields to correct database columns
2. Check for existing record before insert (UNIQUE constraint on student_id)
3. Use PATCH for updates, POST for new records

#### Field Mappings

| Form Field | Database Column |
|------------|-----------------|
| `fafsaCompleted` | `fafsa_completed` |
| `fafsaId` | `efc` (Expected Family Contribution) |
| `employmentStatus` | `employment_status` |
| `employer` | `employer_name` |
| `dependents` | `dependents_count` |
| `scholarshipInterest` | Combined into `aid_types` |
| `workStudyInterest` | Combined into `aid_types` |
| `loanInterest` | Combined into `aid_types` |
| `veteranBenefits` | Combined into `aid_types` |
| `currentScholarships` | `scholarship_applications` |
| `specialCircumstances` | Appended to `scholarship_applications` |

---

### Version 1.3.0 - EmergencyContact Table for Multiple Contacts

**Date:** January 2026
**Purpose:** Support multiple emergency contacts per student with proper API integration.

#### Architecture Change

Emergency contacts are stored in the `emergency_contact` table. This allows:

1. **Multiple contacts per student** - Primary, secondary, etc.
2. **Priority ordering** - Contacts can be ordered by priority
3. **Complete contact information** - Phone, email, address, relationship

#### `emergency_contact` Table Schema

| Column Name | Type | Description |
|-------------|------|-------------|
| `id` | SERIAL | Primary key |
| `student_id` | INTEGER | Foreign key to student table |
| `first_name` | VARCHAR(100) | Contact's first name |
| `last_name` | VARCHAR(100) | Contact's last name |
| `contact_relationship` | VARCHAR(100) | Relationship to student |
| `phone` | VARCHAR(30) | Primary phone |
| `alternate_phone` | VARCHAR(30) | Alternate phone |
| `email` | VARCHAR(200) | Email address |
| `street1` | VARCHAR(200) | Street address line 1 |
| `street2` | VARCHAR(200) | Street address line 2 |
| `city` | VARCHAR(100) | City |
| `state` | VARCHAR(100) | State/Province |
| `postal_code` | VARCHAR(20) | ZIP/Postal code |
| `country` | VARCHAR(100) | Country |
| `is_primary` | BOOLEAN | Whether this is the primary contact |
| `priority` | INTEGER | Contact priority (1 = highest) |

#### New C++ Classes

- **`Models::EmergencyContact`** - Model class for emergency contacts
  - `src/models/EmergencyContact.h`
  - `src/models/EmergencyContact.cpp`

#### New API Methods in `FormSubmissionService`

- `getEmergencyContacts(studentId)` - Get all contacts for a student
- `createEmergencyContact(contact)` - Create new contact
- `updateEmergencyContact(contact)` - Update existing contact
- `deleteEmergencyContact(contactId)` - Delete contact
- `saveEmergencyContact(contact)` - Create or update

#### Form Integration

The `EmergencyContactForm` now:
- Loads existing contacts from API on form display
- Saves contacts to API on form submission
- Pre-fills contact fields when returning to the form

---

### Version 1.2.0 - StudentAddress Table for Multiple Addresses

**Date:** January 2026
**Purpose:** Support multiple addresses per student (home, mailing, billing) using the `student_address` table.

#### Architecture Change

Addresses are now stored in the `student_address` table instead of directly on the `student` table. This allows:

1. **Multiple addresses per student** - Home, mailing, billing addresses
2. **Address history** - Track address changes over time
3. **Proper normalization** - One-to-many relationship between student and addresses

#### `student_address` Table Schema

| Column Name | Type | Description |
|-------------|------|-------------|
| `id` | SERIAL | Primary key |
| `student_id` | INTEGER | Foreign key to student table |
| `address_type` | VARCHAR(50) | Type: 'permanent', 'mailing', 'billing' |
| `street1` | VARCHAR(200) | Street address line 1 |
| `street2` | VARCHAR(200) | Street address line 2 |
| `city` | VARCHAR(100) | City |
| `state` | VARCHAR(100) | State/Province |
| `postal_code` | VARCHAR(20) | ZIP/Postal code |
| `country` | VARCHAR(100) | Country |
| `is_primary` | BOOLEAN | Whether this is the primary address |

#### New C++ Classes

- **`Models::StudentAddress`** - Model class for student addresses
  - `src/models/StudentAddress.h`
  - `src/models/StudentAddress.cpp`

#### New API Methods in `FormSubmissionService`

- `getStudentAddresses(studentId)` - Get all addresses for a student
- `getStudentAddress(studentId, addressType)` - Get specific address by type
- `createStudentAddress(address)` - Create new address
- `updateStudentAddress(address)` - Update existing address
- `deleteStudentAddress(addressId)` - Delete address
- `saveStudentAddress(address)` - Create or update (upsert)

---

### Version 1.1.0 - Student Personal Information Fields

**Date:** January 2026
**Purpose:** Add fields to support pre-filling of Personal Information form data for returning students.

#### New Columns Added to `student` Table

| Column Name | Type | Description |
|-------------|------|-------------|
| `preferred_pronouns` | VARCHAR(50) | Student's preferred pronouns |
| `phone_number` | VARCHAR(30) | Primary phone number |
| `ssn` | VARCHAR(20) | Social Security Number |
| `citizenship_status` | VARCHAR(50) | Citizenship status |
| `completed_forms` | TEXT | JSON array of completed form IDs |

---

## Migration Instructions

### Prerequisites

- Access to the PostgreSQL database
- ApiLogicServer CLI installed
- Application source code

### Step 1: Backup the Database

```bash
pg_dump -U your_user -d student_intake -f backup_before_migration.sql
```

### Step 2: Run the SQL Migration

Execute the migration script on your database:

```bash
psql -U your_user -d student_intake -f database/migrations/001_add_student_fields.sql
```

Or run the SQL directly:

```sql
-- PostgreSQL Migration
ALTER TABLE student ADD COLUMN IF NOT EXISTS preferred_pronouns VARCHAR(50);
ALTER TABLE student ADD COLUMN IF NOT EXISTS ssn VARCHAR(20);
ALTER TABLE student ADD COLUMN IF NOT EXISTS citizenship_status VARCHAR(50);
ALTER TABLE student ADD COLUMN IF NOT EXISTS completed_forms TEXT;
ALTER TABLE student ADD COLUMN IF NOT EXISTS phone_number VARCHAR(30);

-- The student_address table should already exist in the schema
-- It stores addresses with address_type: 'permanent', 'mailing', 'billing'
```

### Step 3: Regenerate ApiLogicServer Models

After modifying the database schema, regenerate the ApiLogicServer project to update the API models:

```bash
cd /path/to/api-server
ApiLogicServer create --project_name=. --db_url=postgresql://user:pass@localhost/student_intake
```

**Important:** This regenerates the models from the database schema. If you have custom logic in `logic/` or `api/` directories, ensure they are preserved or backed up.

### Step 4: Restart ApiLogicServer

```bash
cd /path/to/api-server
python api_logic_server_run.py
```

### Step 5: Rebuild the C++ Application

```bash
cd /path/to/Student-Onboarding
mkdir -p build && cd build
cmake ..
make -j$(nproc)
```

### Step 6: Verify the Migration

1. Start the application
2. Log in as an existing student
3. Navigate to Personal Information form
4. Enter data in all fields (address, SSN, citizenship status)
5. Submit the form
6. Log out and log back in
7. Verify all fields are pre-populated

---

## Schema Reference

### Full `student` Table Schema (v1.2.0)

```sql
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
```

### Full `student_address` Table Schema

```sql
CREATE TABLE student_address (
    id SERIAL PRIMARY KEY,
    student_id INTEGER NOT NULL REFERENCES student(id),
    address_type VARCHAR(50) NOT NULL,  -- 'permanent', 'mailing', 'billing'
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
```

---

## Field Mappings

### API to Database Field Names

The C++ application uses camelCase internally, while the database uses snake_case. Models handle this conversion automatically.

| C++ Model Field | Database Column | JSON API Field |
|-----------------|-----------------|----------------|
| `firstName_` | `first_name` | `first_name` |
| `lastName_` | `last_name` | `last_name` |
| `phoneNumber_` | `phone_number` | `phone_number` |
| `ssn_` | `ssn` | `ssn` |
| `citizenshipStatus_` | `citizenship_status` | `citizenship_status` |

### StudentAddress Field Mappings

| C++ Model Field | Database Column | JSON API Field |
|-----------------|-----------------|----------------|
| `studentId_` | `student_id` | `student_id` |
| `addressType_` | `address_type` | `address_type` |
| `street1_` | `street1` | `street1` |
| `street2_` | `street2` | `street2` |
| `city_` | `city` | `city` |
| `state_` | `state` | `state` |
| `postalCode_` | `postal_code` | `postal_code` |
| `country_` | `country` | `country` |
| `isPrimary_` | `is_primary` | `is_primary` |

### EmergencyContact Field Mappings

| C++ Model Field | Database Column | JSON API Field |
|-----------------|-----------------|----------------|
| `studentId_` | `student_id` | `student_id` |
| `firstName_` | `first_name` | `first_name` |
| `lastName_` | `last_name` | `last_name` |
| `relationship_` | `contact_relationship` | `contact_relationship` |
| `phone_` | `phone` | `phone` |
| `alternatePhone_` | `alternate_phone` | `alternate_phone` |
| `email_` | `email` | `email` |
| `street1_` | `street1` | `street1` |
| `city_` | `city` | `city` |
| `state_` | `state` | `state` |
| `postalCode_` | `postal_code` | `postal_code` |
| `isPrimary_` | `is_primary` | `is_primary` |
| `priority_` | `priority` | `priority` |

### AcademicHistory Field Mappings

| C++ Model Field | Database Column | JSON API Field |
|-----------------|-----------------|----------------|
| `studentId_` | `student_id` | `student_id` |
| `institutionName_` | `institution_name` | `institution_name` |
| `institutionType_` | `institution_type` | `institution_type` |
| `institutionCity_` | `institution_city` | `institution_city` |
| `institutionState_` | `institution_state` | `institution_state` |
| `institutionCountry_` | `institution_country` | `institution_country` |
| `degreeEarned_` | `degree_earned` | `degree_earned` |
| `major_` | `major` | `major` |
| `minor_` | `minor` | `minor` |
| `gpa_` | `gpa` | `gpa` |
| `gpaScale_` | `gpa_scale` | `gpa_scale` |
| `startDate_` | `start_date` | `start_date` |
| `endDate_` | `end_date` | `end_date` |
| `graduationDate_` | `graduation_date` | `graduation_date` |
| `isCurrentlyAttending_` | `is_currently_attending` | `is_currently_attending` |
| `transcriptReceived_` | `transcript_received` | `transcript_received` |

---

## Troubleshooting

### Fields not being saved

1. Verify the database columns exist:
   ```sql
   SELECT column_name FROM information_schema.columns WHERE table_name = 'student';
   SELECT column_name FROM information_schema.columns WHERE table_name = 'student_address';
   ```

2. Verify ApiLogicServer models were regenerated (check `database/models.py`)

3. Check API logs for any field mapping errors

### Addresses not being retrieved

1. Verify addresses exist in student_address table:
   ```sql
   SELECT * FROM student_address WHERE student_id = <your_student_id>;
   ```

2. Check that `address_type` is set to 'permanent' or 'mailing'

3. Verify the API endpoint `/StudentAddress` is accessible

### Pre-fill not working for specific fields

1. Verify the form's `loadAddressesFromApi()` method is being called
2. Check that `saveAddressesToApi()` is called in `handleSubmit()`
3. Verify the API returns the correct `address_type` values

---

## Related Files

### Database
- `database/schema.sql` - PostgreSQL database schema (snake_case naming)
- `database/migrations/001_add_student_fields.sql` - Migration script

### Models
- `src/models/Student.h/.cpp` - Student model
- `src/models/StudentAddress.h/.cpp` - StudentAddress model (multiple addresses)
- `src/models/EmergencyContact.h/.cpp` - EmergencyContact model (multiple contacts)
- `src/models/AcademicHistory.h/.cpp` - AcademicHistory model (multiple records)
- `src/models/FormData.h/.cpp` - Form data container
- `src/models/Curriculum.h/.cpp` - Curriculum model

### API Service
- `src/api/FormSubmissionService.h` - API service declarations
- `src/api/FormSubmissionService.cpp` - API service implementation with:
  - Student CRUD methods
  - StudentAddress CRUD methods
  - EmergencyContact CRUD methods
  - AcademicHistory CRUD methods
  - Form submission methods (with field mapping)

### Forms
- `src/forms/PersonalInfoForm.cpp` - Personal info with address load/save
- `src/forms/EmergencyContactForm.cpp` - Emergency contacts load/save
- `src/forms/AcademicHistoryForm.cpp` - Academic history load/save
- `src/forms/FinancialAidForm.cpp` - Financial aid form

### Build Configuration
- `CMakeLists.txt` - Build configuration (includes all model .cpp files)

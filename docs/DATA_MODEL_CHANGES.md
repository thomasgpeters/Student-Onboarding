# Data Model Changes

This document tracks changes to the database schema and provides migration instructions for the Student Onboarding application.

## Overview

The application uses a PostgreSQL database with ApiLogicServer as the middleware layer. When schema changes are made, both the database and ApiLogicServer must be updated.

---

## Change Log

### Version 1.1.0 - Student Personal Information Fields

**Date:** January 2026
**Purpose:** Add fields to support pre-filling of Personal Information form data for returning students.

#### New Columns Added to `student` Table

| Column Name | Type | Description |
|-------------|------|-------------|
| `preferred_pronouns` | VARCHAR(50) | Student's preferred pronouns (e.g., he/him, she/her, they/them) |
| `phone_number` | VARCHAR(30) | Primary phone number (renamed from `phone`) |
| `address_line1` | VARCHAR(200) | Street address line 1 |
| `address_line2` | VARCHAR(200) | Street address line 2 (apt, suite, etc.) |
| `city` | VARCHAR(100) | City |
| `state` | VARCHAR(100) | State/Province |
| `zip_code` | VARCHAR(20) | ZIP/Postal code |
| `ssn` | VARCHAR(20) | Social Security Number (encrypted at rest) |
| `citizenship_status` | VARCHAR(50) | Citizenship status (U.S. Citizen, Permanent Resident, etc.) |
| `completed_forms` | TEXT | JSON array of completed form IDs for tracking progress |

#### Rationale

Previously, address and citizenship information was stored in separate tables (`student_address`) or not stored at all. To support form pre-fill functionality for returning students, these fields were added directly to the `student` table for:

1. **Simpler data retrieval** - Single query returns all personal info
2. **Form pre-population** - Returning students see their previously entered data
3. **Session persistence** - Track which forms have been completed across login sessions

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
ALTER TABLE student ADD COLUMN IF NOT EXISTS address_line1 VARCHAR(200);
ALTER TABLE student ADD COLUMN IF NOT EXISTS address_line2 VARCHAR(200);
ALTER TABLE student ADD COLUMN IF NOT EXISTS city VARCHAR(100);
ALTER TABLE student ADD COLUMN IF NOT EXISTS state VARCHAR(100);
ALTER TABLE student ADD COLUMN IF NOT EXISTS zip_code VARCHAR(20);
ALTER TABLE student ADD COLUMN IF NOT EXISTS ssn VARCHAR(20);
ALTER TABLE student ADD COLUMN IF NOT EXISTS citizenship_status VARCHAR(50);
ALTER TABLE student ADD COLUMN IF NOT EXISTS completed_forms TEXT;

-- If renaming phone to phone_number (optional, for new installations)
-- ALTER TABLE student RENAME COLUMN phone TO phone_number;
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

### Full `student` Table Schema (v1.1.0)

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
```

---

## Field Mappings

### API to Database Field Names

The C++ application uses camelCase internally, while the database uses snake_case. The `Student` model handles this conversion automatically.

| C++ Model Field | Database Column | JSON API Field |
|-----------------|-----------------|----------------|
| `firstName_` | `first_name` | `first_name` |
| `lastName_` | `last_name` | `last_name` |
| `middleName_` | `middle_name` | `middle_name` |
| `preferredName_` | `preferred_name` | `preferred_name` |
| `preferredPronouns_` | `preferred_pronouns` | `preferred_pronouns` |
| `phoneNumber_` | `phone_number` | `phone_number` |
| `alternatePhone_` | `alternate_phone` | `alternate_phone` |
| `addressLine1_` | `address_line1` | `address_line1` |
| `addressLine2_` | `address_line2` | `address_line2` |
| `city_` | `city` | `city` |
| `state_` | `state` | `state` |
| `zipCode_` | `zip_code` | `zip_code` |
| `ssn_` | `ssn` | `ssn` |
| `citizenshipStatus_` | `citizenship_status` | `citizenship_status` |
| `completedForms_` | `completed_forms` | `completed_forms` |

---

## Troubleshooting

### Fields not being saved

1. Verify the database columns exist:
   ```sql
   SELECT column_name FROM information_schema.columns WHERE table_name = 'student';
   ```

2. Verify ApiLogicServer models were regenerated (check `database/models.py`)

3. Check API logs for any field mapping errors

### Fields not being retrieved

1. Verify the `Student::fromJson()` method handles both camelCase and snake_case field names

2. Check that `loginStudent()` returns the full student record with all attributes

### Pre-fill not working for specific fields

1. Verify the form's `createFormFields()` method reads from the correct getter methods
2. Check that `updateStudentFromForm()` sets all fields before calling `updateStudentProfile()`

---

## Related Files

- `database/postgresql-snake-case.sql` - Full PostgreSQL schema
- `database/schema.sql` - SQLite schema (for development)
- `database/migrations/001_add_student_fields.sql` - Migration script
- `src/models/Student.h` - Student model header
- `src/models/Student.cpp` - Student model implementation (toJson/fromJson)
- `src/forms/PersonalInfoForm.cpp` - Form pre-fill and save logic

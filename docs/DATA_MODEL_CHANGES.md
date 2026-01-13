# Data Model Changes

This document tracks changes to the database schema and provides migration instructions for the Student Onboarding application.

## Overview

The application uses a PostgreSQL database with ApiLogicServer as the middleware layer. When schema changes are made, both the database and ApiLogicServer must be updated.

---

## Change Log

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

- `database/postgresql-snake-case.sql` - Full PostgreSQL schema
- `database/schema.sql` - SQLite schema (for development)
- `database/migrations/001_add_student_fields.sql` - Migration script
- `src/models/Student.h` - Student model header
- `src/models/Student.cpp` - Student model implementation
- `src/models/StudentAddress.h` - StudentAddress model header
- `src/models/StudentAddress.cpp` - StudentAddress model implementation
- `src/api/FormSubmissionService.h` - API service with address methods
- `src/api/FormSubmissionService.cpp` - API service implementation
- `src/forms/PersonalInfoForm.cpp` - Form with address load/save logic

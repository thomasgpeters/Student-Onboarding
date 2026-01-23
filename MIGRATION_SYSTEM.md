# Student Onboarding Migration System

This document provides comprehensive documentation for the program mode migration system, enabling institutions to switch between **Accredited** (university/college) and **Vocational** (trade school/CDL) configurations.

---

## Table of Contents

1. [Overview](#overview)
2. [System Architecture](#system-architecture)
3. [Program Modes](#program-modes)
4. [Migration Scripts](#migration-scripts)
5. [Database Schema](#database-schema)
6. [Quick Start Guide](#quick-start-guide)
7. [Detailed Migration Process](#detailed-migration-process)
8. [Data Preservation](#data-preservation)
9. [Rollback Procedures](#rollback-procedures)
10. [Customization](#customization)
11. [Troubleshooting](#troubleshooting)
12. [API Integration](#api-integration)

---

## Overview

The Student Onboarding system supports multiple institution types through a flexible migration system. This allows a single codebase to serve:

- **Universities and Colleges** - Traditional academic programs with degrees (Associate, Bachelor, Master, Doctoral)
- **Trade Schools** - Vocational training programs with certificates
- **CDL Training Centers** - Commercial Driver's License programs with endorsements
- **Hybrid Institutions** - Combination of academic and vocational programs

### Key Features

- **One-command switching** between program modes
- **Data preservation** for students and form submissions
- **Automatic foreign key handling** during migrations
- **Institution settings synchronization**
- **Rollback capability** with database backups

---

## System Architecture

### File Structure

```
database/
├── schema.sql                    # Complete database schema (all tables, indexes, triggers)
├── install.sql                   # Single installation script (schema + seed data)
│
├── scripts/
│   ├── switch_to_accredited.sql  # Complete accredited/academic mode switch
│   ├── switch_to_vocational.sql  # Complete vocational/trade school mode switch
│   └── README.md                 # Scripts documentation
│
└── migrations/
    ├── 003_add_endorsement_support.sql  # Upgrade: adds multi-program enrollment
    └── archive/                  # Historical migration files (reference only)
        ├── README.md
        ├── 001_add_student_fields.sql
        ├── 002_add_admin_tables.sql
        └── ... (additional archived files)
```

> **Note:** Individual migration files have been consolidated into `schema.sql` and `install.sql` for simpler deployments. The archived migrations are kept for historical reference only.
>
> **Upgrades:** The `migrations/` folder contains upgrade scripts for existing databases. Run these in numerical order when upgrading.

### Settings Architecture

The migration system uses the `institution_settings` table to track configuration:

| Setting Key | Description | Values |
|-------------|-------------|--------|
| `institution_type` | Type of institution | `accredited`, `vocational` |
| `program_mode` | Current program configuration | `accredited`, `vocational`, `hybrid` |
| `degree_types_enabled` | JSON array of enabled degrees | `["bachelor", "master", ...]` |

---

## Program Modes

### Accredited Mode

Designed for traditional academic institutions offering degree programs.

#### Departments

| Code | Name | Contact |
|------|------|---------|
| CS | Computer Science | cs@university.edu |
| BUS | Business Administration | business@university.edu |
| ENG | Engineering | engineering@university.edu |
| NUR | Nursing | nursing@university.edu |
| A&S | Arts and Sciences | artsci@university.edu |

#### Programs

| Code | Name | Degree | Duration |
|------|------|--------|----------|
| cs_bs | Bachelor of Science in Computer Science | Bachelor | 8 Semesters |
| cs_ms | Master of Science in Computer Science | Master | 4 Semesters |
| cert_da | Certificate in Data Analytics | Certificate | 2 Semesters |
| bus_bba | Bachelor of Business Administration | Bachelor | 8 Semesters |
| bus_mba | Master of Business Administration | Master | 4 Semesters |
| cert_pm | Certificate in Project Management | Certificate | 2 Semesters |
| eng_bsee | BS in Electrical Engineering | Bachelor | 8 Semesters |
| eng_bsme | BS in Mechanical Engineering | Bachelor | 8 Semesters |
| nur_bsn | Bachelor of Science in Nursing | Bachelor | 8 Semesters |
| art_ba | Bachelor of Arts in Psychology | Bachelor | 8 Semesters |

#### Degree Types Available
- Associate
- Bachelor
- Master
- Doctoral
- Certificate

#### Duration Interval
- Primary: **Semester**
- Example: "8 Semesters" for a 4-year degree

---

### Vocational Mode

Designed for trade schools, CDL training centers, and vocational institutions.

**Multi-Program Enrollment:** Students select one base program (e.g., Class A CDL) and can add multiple endorsements (e.g., Tanker, HazMat). See [Migration 003](#upgrading-existing-databases) for database requirements.

#### Departments

| Code | Name | Contact |
|------|------|---------|
| CDL | Professional Driving | driving@school.edu |
| AUTO | Automotive Technology | automotive@school.edu |
| FOOD | Food Services | culinary@school.edu |
| TRADE | Skilled Trades | trades@school.edu |
| ELECT | Electrical Technology | electrical@school.edu |
| WELD | Welding Technology | welding@school.edu |
| HVAC | HVAC Technology | hvac@school.edu |
| MED | Medical Services | medical@school.edu |

#### CDL Programs - Class A

| Code | Name | Duration | Endorsements |
|------|------|----------|--------------|
| cdl_class_a | Class A CDL Training Program | 4 Weeks | Base |
| cdl_class_a_doubles_triples | Doubles/Triples Endorsement (T) | 1 Week | T |
| cdl_class_a_tanker | Tanker Endorsement (N) | 1 Week | N |
| cdl_class_a_hazmat | Hazardous Materials Endorsement (H) | 1 Week | H |
| cdl_class_a_air_brakes | Air Brakes Training | 3 Days | air_brakes |

#### CDL Programs - Class B

| Code | Name | Duration | Endorsements |
|------|------|----------|--------------|
| cdl_class_b | Class B CDL Training Program | 3 Weeks | Base |
| cdl_class_b_passenger | Passenger Endorsement (P) | 1 Week | P |
| cdl_class_b_school_bus | School Bus Endorsement (S) | 1 Week | S |
| cdl_class_b_air_brakes | Air Brakes Training | 3 Days | air_brakes |

#### Degree Types Available
- Certificate (only)

#### Duration Intervals
- Primary: **Week**, **Day**
- Examples: "4 Weeks", "3 Days"

---

## Migration Scripts

### switch_to_accredited.sql

**Location:** `database/scripts/switch_to_accredited.sql`

**Purpose:** Completely transitions the system to accredited/academic mode.

**Operations performed:**
1. Clears `curriculum_form_requirement` table
2. Clears `curriculum_prerequisite` table
3. Nullifies `curriculum_id` on all student records
4. Clears `curriculum` table
5. Clears `department` table
6. Resets database sequences
7. Inserts 5 academic departments
8. Inserts 10 academic programs
9. Inserts program prerequisites
10. Inserts form requirements
11. Updates institution settings

**Usage:**
```bash
psql -U postgres -d student_onboarding -f database/scripts/switch_to_accredited.sql
```

---

### switch_to_vocational.sql

**Location:** `database/scripts/switch_to_vocational.sql`

**Purpose:** Completely transitions the system to vocational/trade school mode.

**Operations performed:**
1. Clears `curriculum_form_requirement` table
2. Clears `curriculum_prerequisite` table
3. Nullifies `curriculum_id` on all student records
4. Clears `curriculum` table
5. Clears `department` table
6. Resets database sequences
7. Inserts 8 vocational departments
8. Inserts 9 CDL programs (5 Class A, 4 Class B)
9. Inserts program prerequisites
10. Inserts form requirements
11. Updates institution settings

**Usage:**
```bash
psql -U postgres -d student_onboarding -f database/scripts/switch_to_vocational.sql
```

---

## Database Schema

### Institution Settings

```sql
CREATE TABLE institution_settings (
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
```

### Curriculum (Extended for Vocational)

```sql
-- Key fields for vocational support
ALTER TABLE curriculum ADD COLUMN IF NOT EXISTS duration_interval VARCHAR(20) DEFAULT 'semester';
ALTER TABLE curriculum ADD COLUMN IF NOT EXISTS is_accredited BOOLEAN DEFAULT TRUE;
ALTER TABLE curriculum ADD COLUMN IF NOT EXISTS program_type VARCHAR(50) DEFAULT 'academic';
ALTER TABLE curriculum ADD COLUMN IF NOT EXISTS cdl_class VARCHAR(10);
ALTER TABLE curriculum ADD COLUMN IF NOT EXISTS endorsements JSONB DEFAULT '[]';
ALTER TABLE curriculum ADD COLUMN IF NOT EXISTS training_hours_classroom INTEGER;
ALTER TABLE curriculum ADD COLUMN IF NOT EXISTS training_hours_range INTEGER;
ALTER TABLE curriculum ADD COLUMN IF NOT EXISTS training_hours_road INTEGER;
```

### Duration Interval Constraint

```sql
ALTER TABLE curriculum ADD CONSTRAINT curriculum_duration_interval_check
    CHECK (duration_interval IN ('semester', 'month', 'week', 'day'));
```

---

## Quick Start Guide

### Prerequisites

- PostgreSQL database
- `psql` command-line client
- Database user with appropriate permissions

### New Installation

For new installations, use the consolidated install script:

```bash
# Create database
createdb student_onboarding

# Run installation (creates schema, seeds form types, settings, and admin user)
psql -U postgres -d student_onboarding -f database/install.sql

# Choose curriculum mode (required):
# For universities/colleges (accredited):
psql -U postgres -d student_onboarding -f database/scripts/switch_to_accredited.sql

# OR for trade schools/CDL (vocational):
psql -U postgres -d student_onboarding -f database/scripts/switch_to_vocational.sql
```

### Upgrading Existing Databases

For existing installations, run applicable migration scripts in numerical order:

```bash
# Check your current schema version by looking at what tables/columns exist
psql -U postgres -d student_onboarding -c "\d curriculum" | grep is_endorsement

# If is_endorsement column doesn't exist, run migration 003:
psql -U postgres -d student_onboarding -f database/migrations/003_add_endorsement_support.sql

# Then re-run the curriculum mode script to populate endorsement data:
psql -U postgres -d student_onboarding -f database/scripts/switch_to_vocational.sql
```

**Migration 003: Multi-Program Endorsement Support**
- Adds `is_endorsement` column to `curriculum` table
- Creates `student_endorsement` junction table
- Enables vocational students to enroll in base program + multiple endorsements

---

### Step 1: Backup Your Database

**Always backup before switching modes!**

```bash
# Full database backup
pg_dump -U postgres -d student_onboarding > backup_$(date +%Y%m%d_%H%M%S).sql

# Or backup specific tables
pg_dump -U postgres -d student_onboarding -t curriculum -t department > curriculum_backup.sql
```

### Step 2: Switch Mode

```bash
cd database/scripts

# For university/college setup
psql -U postgres -d student_onboarding -f switch_to_accredited.sql

# OR for trade school/CDL setup
psql -U postgres -d student_onboarding -f switch_to_vocational.sql
```

### Step 4: Verify

```sql
-- Check current mode
SELECT setting_key, setting_value
FROM institution_settings
WHERE setting_key IN ('institution_type', 'program_mode', 'degree_types_enabled');

-- Check departments loaded
SELECT code, name FROM department ORDER BY code;

-- Check programs loaded
SELECT code, name, degree_type, duration_semesters, duration_interval
FROM curriculum ORDER BY code;
```

---

## Detailed Migration Process

### Execution Order

The switch scripts execute in this specific order to respect foreign key constraints:

```
1. DELETE FROM curriculum_form_requirement
   └── Removes links between curricula and required forms

2. DELETE FROM curriculum_prerequisite
   └── Removes prerequisite relationships

3. UPDATE student SET curriculum_id = NULL
   └── Detaches students from programs (preserves student records)

4. DELETE FROM curriculum
   └── Removes all program definitions

5. DELETE FROM department
   └── Removes all department definitions

6. ALTER SEQUENCE ... RESTART WITH 1
   └── Resets ID sequences for clean numbering

7. INSERT INTO department ...
   └── Loads new departments for target mode

8. INSERT INTO curriculum ...
   └── Loads new programs for target mode

9. INSERT INTO curriculum_prerequisite ...
   └── Establishes prerequisite relationships

10. INSERT INTO curriculum_form_requirement ...
    └── Links programs to required intake forms

11. UPDATE institution_settings ...
    └── Updates configuration for target mode
```

### Conflict Handling

All INSERT statements use `ON CONFLICT` clauses for idempotency:

```sql
INSERT INTO department (code, name, ...)
VALUES (...)
ON CONFLICT (code) DO UPDATE SET
    name = EXCLUDED.name,
    updated_at = CURRENT_TIMESTAMP;
```

This allows scripts to be safely re-run without errors.

---

## Data Preservation

### What IS Preserved

| Data Type | Preserved? | Notes |
|-----------|------------|-------|
| Student records | ✅ Yes | `curriculum_id` set to NULL |
| Form submissions | ✅ Yes | All submitted forms intact |
| Admin users | ✅ Yes | No changes |
| Form types | ✅ Yes | Form definitions unchanged |
| Institution settings | ✅ Yes | Updated to new mode |

### What IS NOT Preserved

| Data Type | Preserved? | Notes |
|-----------|------------|-------|
| Departments | ❌ No | Replaced with mode-specific |
| Curriculum | ❌ No | Replaced with mode-specific |
| Form requirements | ❌ No | Re-linked to new programs |
| Prerequisites | ❌ No | Re-established for new programs |

### Student Re-assignment

After switching modes, students must be reassigned to new programs:

**Via Admin UI:**
1. Navigate to Student Management
2. Edit each student
3. Select appropriate program from dropdown

**Via SQL:**
```sql
-- Reassign student to a specific program
UPDATE student
SET curriculum_id = (SELECT id FROM curriculum WHERE code = 'cdl_class_a')
WHERE id = 123;

-- Bulk reassign by program type
UPDATE student s
SET curriculum_id = (SELECT id FROM curriculum WHERE code = 'cdl_class_a' LIMIT 1)
WHERE s.curriculum_id IS NULL;
```

---

## Rollback Procedures

### From Backup

```bash
# Drop and recreate database
dropdb student_onboarding
createdb student_onboarding

# Restore from backup
psql -U postgres -d student_onboarding < backup_20240115_143022.sql
```

### Switch to Other Mode

Simply run the other mode's switch script:

```bash
# If currently vocational, switch to accredited
psql -U postgres -d student_onboarding -f switch_to_accredited.sql

# If currently accredited, switch to vocational
psql -U postgres -d student_onboarding -f switch_to_vocational.sql
```

---

## Customization

### Adding Custom Departments

After switching modes, add custom departments:

```sql
INSERT INTO department (code, name, dean, contact_email)
VALUES ('CUSTOM', 'Custom Department', 'Jane Doe', 'custom@school.edu');
```

### Adding Custom Programs

```sql
INSERT INTO curriculum (
    code, name, description, department_id, degree_type,
    credit_hours, duration_semesters, duration_interval,
    is_active, is_online, is_accredited, program_type
)
VALUES (
    'custom_cert', 'Custom Certificate Program',
    'Description of the custom program',
    (SELECT id FROM department WHERE code = 'CUSTOM'),
    'certificate', 120, 6, 'month',
    TRUE, FALSE, FALSE, 'vocational'
);
```

### Modifying Degree Types

Update enabled degree types via settings:

```sql
-- Add doctoral to vocational mode
UPDATE institution_settings
SET setting_value = '["certificate", "associate"]'
WHERE setting_key = 'degree_types_enabled';
```

### Creating Hybrid Mode

For institutions offering both academic and vocational programs:

1. Run either switch script as base
2. Add programs from the other mode manually
3. Update settings:

```sql
UPDATE institution_settings
SET setting_value = 'hybrid'
WHERE setting_key = 'program_mode';

UPDATE institution_settings
SET setting_value = '["associate", "bachelor", "master", "certificate"]'
WHERE setting_key = 'degree_types_enabled';
```

---

## Troubleshooting

### Error: Foreign Key Violation

**Symptom:** Error when deleting from curriculum or department tables.

**Cause:** Data still references these tables.

**Solution:** Ensure deletion order is correct:
```sql
DELETE FROM curriculum_form_requirement;
DELETE FROM curriculum_prerequisite;
UPDATE student SET curriculum_id = NULL;
DELETE FROM curriculum;
DELETE FROM department;
```

### Error: Sequence Does Not Exist

**Symptom:** `ALTER SEQUENCE ... does not exist`

**Cause:** Sequences may have different names depending on PostgreSQL version.

**Solution:** Check actual sequence names:
```sql
SELECT sequence_name FROM information_schema.sequences
WHERE sequence_schema = 'public';
```

### Students Missing Program Assignment

**Symptom:** Students show "No Program" after migration.

**Cause:** Expected behavior - `curriculum_id` is nullified during switch.

**Solution:** Reassign students to new programs via Admin UI or SQL.

### Duration Shows Wrong Format

**Symptom:** Duration shows "2 semesters" instead of "2 Weeks".

**Cause:** `duration_interval` field not set correctly.

**Solution:**
```sql
UPDATE curriculum SET duration_interval = 'week' WHERE code LIKE 'cdl%';
```

### Missing Institution Type Settings

**Symptom:** Settings not appearing in admin panel.

**Cause:** Database not properly initialized.

**Solution:**
```bash
# For new installations, run the full install script
psql -U postgres -d student_onboarding -f database/install.sql

# Then apply your curriculum mode
psql -U postgres -d student_onboarding -f database/scripts/switch_to_vocational.sql
```

---

## API Integration

### Checking Current Mode

```bash
curl -X GET "http://localhost:5656/api/InstitutionSetting?filter[setting_key]=program_mode"
```

### Listing Available Programs

```bash
curl -X GET "http://localhost:5656/api/Curriculum?filter[is_active]=true"
```

### Getting Program with Duration

The API returns `duration_interval` field; use `getFormattedDuration()` in the C++ frontend to display formatted strings like "4 Weeks" or "8 Semesters".

---

## Version History

| Version | Date | Changes |
|---------|------|---------|
| 1.0 | 2024-01 | Initial migration system |
| 1.1 | 2024-01 | Added duration_interval support |
| 1.2 | 2024-01 | Added institution_type settings |
| 2.4.0 | 2025-01 | Added multi-program endorsement support (migration 003) |

---

## Support

For issues with the migration system:

1. Check the [Troubleshooting](#troubleshooting) section
2. Review PostgreSQL logs for detailed error messages
3. Ensure all migrations are applied in order
4. Verify database user has sufficient permissions

---

*This document is part of the Student Onboarding System documentation.*

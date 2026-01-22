# Database Documentation

Comprehensive documentation for the Student Onboarding System database.

## Table of Contents

1. [Overview](#overview)
2. [Quick Start](#quick-start)
3. [Schema Overview](#schema-overview)
4. [Tables Reference](#tables-reference)
5. [Installation](#installation)
6. [Curriculum Modes](#curriculum-modes)
7. [Maintenance](#maintenance)

---

## Overview

The Student Onboarding System uses PostgreSQL as its database backend. The schema supports:

- Student registration and form submissions
- Multiple institution types (universities, trade schools, CDL centers)
- Administrative user management
- Configurable institution settings
- Curriculum/program management

### Database Files

| File | Purpose |
|------|---------|
| `database/schema.sql` | Complete database schema (tables, indexes, triggers) |
| `database/install.sql` | Single installation script with seed data |
| `database/scripts/switch_to_accredited.sql` | Switch to university/college mode |
| `database/scripts/switch_to_vocational.sql` | Switch to trade school/CDL mode |

---

## Quick Start

### New Installation

```bash
# 1. Create database
createdb student_onboarding

# 2. Run installation
psql -U postgres -d student_onboarding -f database/install.sql

# 3. Choose curriculum mode:
# For universities/colleges:
psql -U postgres -d student_onboarding -f database/scripts/switch_to_accredited.sql

# For trade schools/CDL centers:
psql -U postgres -d student_onboarding -f database/scripts/switch_to_vocational.sql
```

### Default Admin User

After installation, the default admin account is:
- **Email:** admin@institution.edu
- **Password:** admin123 (change immediately!)

---

## Schema Overview

### Entity Relationship Diagram (Simplified)

```
┌─────────────────┐     ┌─────────────────┐
│   department    │────<│   curriculum    │
└─────────────────┘     └────────┬────────┘
                                 │
                                 │ curriculum_id
                                 ↓
┌─────────────────┐     ┌─────────────────┐     ┌─────────────────┐
│  form_type      │────<│ form_submission │>────│    student      │
└─────────────────┘     └─────────────────┘     └────────┬────────┘
                                                         │
                        ┌────────────────────────────────┼────────────────────────────────┐
                        │                                │                                │
                        ↓                                ↓                                ↓
              ┌─────────────────┐            ┌─────────────────┐            ┌─────────────────┐
              │emergency_contact│            │ academic_history │            │  financial_aid  │
              └─────────────────┘            └─────────────────┘            └─────────────────┘
```

### Table Categories

| Category | Tables |
|----------|--------|
| Core | `student`, `form_type`, `form_submission` |
| Curriculum | `department`, `curriculum`, `curriculum_prerequisite`, `curriculum_form_requirement` |
| Student Data | `emergency_contact`, `academic_history`, `financial_aid`, `medical_info`, `consent` |
| Admin | `admin_user`, `admin_session` |
| Settings | `institution_settings` |

---

## Tables Reference

### student

Primary table for student records.

| Column | Type | Description |
|--------|------|-------------|
| `id` | SERIAL PK | Unique identifier |
| `email` | VARCHAR(255) | Student email (unique, required) |
| `password_hash` | VARCHAR(255) | Bcrypt password hash |
| `first_name` | VARCHAR(100) | First name |
| `last_name` | VARCHAR(100) | Last name |
| `date_of_birth` | DATE | Date of birth |
| `phone` | VARCHAR(30) | Phone number |
| `address_*` | VARCHAR | Address fields |
| `curriculum_id` | INTEGER FK | Selected program |
| `registration_status` | VARCHAR(50) | Status: pending, in_progress, completed |
| `is_international` | BOOLEAN | International student flag |
| `is_transfer` | BOOLEAN | Transfer student flag |
| `is_veteran` | BOOLEAN | Veteran flag |
| `needs_financial_aid` | BOOLEAN | Financial aid flag |
| `created_at` | TIMESTAMP | Record creation time |
| `updated_at` | TIMESTAMP | Last update time |

### emergency_contact

Emergency contacts using compound primary key.

| Column | Type | Description |
|--------|------|-------------|
| `student_id` | INTEGER | FK to student (part of PK) |
| `contact_relationship` | VARCHAR(50) | Relationship (part of PK) |
| `phone` | VARCHAR(30) | Phone number (part of PK) |
| `name` | VARCHAR(200) | Contact name |
| `email` | VARCHAR(255) | Contact email |
| `is_primary` | BOOLEAN | Primary contact flag |

**Primary Key:** `(student_id, contact_relationship, phone)`

### academic_history

Educational history using compound primary key.

| Column | Type | Description |
|--------|------|-------------|
| `student_id` | INTEGER | FK to student (part of PK) |
| `institution_name` | VARCHAR(200) | School name (part of PK) |
| `institution_type` | VARCHAR(50) | Type: high_school, college, etc. (part of PK) |
| `city`, `state`, `country` | VARCHAR | Location |
| `start_date`, `end_date` | DATE | Attendance dates |
| `is_graduated` | BOOLEAN | Graduation status |
| `degree_type`, `degree_name` | VARCHAR | Degree information |
| `gpa` | DECIMAL(3,2) | Grade point average |
| `major`, `minor` | VARCHAR | Fields of study |

**Primary Key:** `(student_id, institution_name, institution_type)`

### financial_aid

Financial aid application data.

| Column | Type | Description |
|--------|------|-------------|
| `id` | SERIAL PK | Unique identifier |
| `student_id` | INTEGER FK | FK to student (unique) |
| `fafsa_completed` | BOOLEAN | FAFSA completion status |
| `fafsa_efc` | INTEGER | Expected Family Contribution |
| `aid_types_requested` | TEXT | Requested aid types (JSON) |
| `scholarship_*` | Various | Scholarship application fields |
| `employer_*` | Various | Employer tuition assistance fields |
| `work_study_*` | Various | Work study preference fields |
| `financial_hardship_*` | Various | Financial hardship documentation |
| `family_contribution_*` | Various | Family contribution details |

### curriculum

Program/course definitions.

| Column | Type | Description |
|--------|------|-------------|
| `id` | SERIAL PK | Unique identifier |
| `code` | VARCHAR(50) | Program code (unique) |
| `name` | VARCHAR(200) | Program name |
| `description` | TEXT | Program description |
| `department_id` | INTEGER FK | FK to department |
| `degree_type` | VARCHAR(50) | associate, bachelor, master, doctoral, certificate |
| `credit_hours` | INTEGER | Total credit hours |
| `duration_semesters` | INTEGER | Duration value |
| `duration_interval` | VARCHAR(20) | semester, month, week, day |
| `is_active` | BOOLEAN | Active status |
| `is_online` | BOOLEAN | Online availability |
| `is_accredited` | BOOLEAN | Accreditation status |
| `program_type` | VARCHAR(50) | academic, vocational, cdl |
| `cdl_class` | VARCHAR(10) | CDL class (A, B) for CDL programs |
| `endorsements` | JSONB | CDL endorsements array |
| `training_hours_*` | INTEGER | Training hour breakdowns |

### institution_settings

Key-value store for institution configuration.

| Column | Type | Description |
|--------|------|-------------|
| `setting_key` | VARCHAR(100) PK | Setting identifier |
| `setting_value` | TEXT | Setting value |
| `setting_type` | VARCHAR(50) | Data type: string, json, boolean, number |
| `category` | VARCHAR(50) | Setting category |
| `display_name` | VARCHAR(200) | Human-readable name |
| `description` | TEXT | Setting description |
| `is_required` | BOOLEAN | Required flag |
| `display_order` | INTEGER | UI display order |

**Key Settings:**

| Key | Description |
|-----|-------------|
| `institution_name` | Institution name |
| `institution_type` | accredited or vocational |
| `program_mode` | accredited, vocational, or hybrid |
| `degree_types_enabled` | JSON array of enabled degree types |
| `contact_email` | Primary contact email |
| `primary_color` | Brand color (hex) |

### admin_user

Administrative user accounts.

| Column | Type | Description |
|--------|------|-------------|
| `id` | SERIAL PK | Unique identifier |
| `email` | VARCHAR(255) | Admin email (unique) |
| `password_hash` | VARCHAR(255) | Bcrypt password hash |
| `first_name` | VARCHAR(100) | First name |
| `last_name` | VARCHAR(100) | Last name |
| `role` | VARCHAR(50) | super_admin, admin, staff |
| `is_active` | BOOLEAN | Account active status |
| `last_login` | TIMESTAMP | Last login time |

---

## Installation

### Prerequisites

- PostgreSQL 12 or later
- `psql` command-line client
- Database user with CREATE privileges

### Full Installation

```bash
# Create database
createdb student_onboarding

# Run complete installation
psql -U postgres -d student_onboarding -f database/install.sql
```

The `install.sql` script:
1. Creates all tables via `schema.sql`
2. Seeds form types (personal_info, emergency_contact, etc.)
3. Seeds institution settings (branding, contact, location)
4. Creates default admin user

### Schema Only

For schema without seed data:

```bash
psql -U postgres -d student_onboarding -f database/schema.sql
```

### Verification

After installation, verify:

```sql
-- Check tables created
SELECT COUNT(*) FROM information_schema.tables
WHERE table_schema = 'public';

-- Check form types seeded
SELECT code, name FROM form_type ORDER BY display_order;

-- Check settings seeded
SELECT setting_key, setting_value FROM institution_settings
WHERE category = 'general';
```

---

## Curriculum Modes

The system supports two curriculum modes. See [MIGRATION_SYSTEM.md](MIGRATION_SYSTEM.md) for detailed documentation.

### Accredited Mode (Universities/Colleges)

```bash
psql -U postgres -d student_onboarding -f database/scripts/switch_to_accredited.sql
```

- 5 academic departments
- 10 degree programs (bachelor, master, certificate)
- Duration measured in semesters

### Vocational Mode (Trade Schools/CDL)

```bash
psql -U postgres -d student_onboarding -f database/scripts/switch_to_vocational.sql
```

- 8 vocational departments
- 9 CDL programs (Class A, Class B)
- Duration measured in weeks/days

### Switching Modes

Modes can be switched at any time. Student records are preserved (program assignment is cleared).

```bash
# Backup first
pg_dump -U postgres -d student_onboarding > backup.sql

# Switch mode
psql -U postgres -d student_onboarding -f database/scripts/switch_to_vocational.sql
```

---

## Maintenance

### Backup

```bash
# Full backup
pg_dump -U postgres -d student_onboarding > backup_$(date +%Y%m%d).sql

# Compressed backup
pg_dump -U postgres -d student_onboarding | gzip > backup_$(date +%Y%m%d).sql.gz
```

### Restore

```bash
# Drop and recreate
dropdb student_onboarding
createdb student_onboarding
psql -U postgres -d student_onboarding < backup.sql
```

### Reset Database

```bash
# Complete reset
dropdb student_onboarding
createdb student_onboarding
psql -U postgres -d student_onboarding -f database/install.sql
psql -U postgres -d student_onboarding -f database/scripts/switch_to_vocational.sql
```

### Index Maintenance

```sql
-- Reindex all tables
REINDEX DATABASE student_onboarding;

-- Analyze for query optimization
ANALYZE;

-- Vacuum to reclaim space
VACUUM ANALYZE;
```

### Check Table Sizes

```sql
SELECT
    relname AS table_name,
    pg_size_pretty(pg_total_relation_size(relid)) AS total_size
FROM pg_catalog.pg_statio_user_tables
ORDER BY pg_total_relation_size(relid) DESC;
```

---

## Related Documentation

- [MIGRATION_SYSTEM.md](MIGRATION_SYSTEM.md) - Curriculum mode switching
- [LOGGING.md](LOGGING.md) - Application logging
- [DOCKER.md](DOCKER.md) - Docker deployment

---

*This document is part of the Student Onboarding System documentation.*

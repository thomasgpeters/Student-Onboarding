```
╔═══════════════════════════════════════════════════════════════╗
║         STUDENT ONBOARD - Curriculum Seeding Guide            ║
╚═══════════════════════════════════════════════════════════════╝
```

# Curriculum Data Seeding Guide

This guide explains how to manage curriculum data in the Student Onboarding system, including how to seed, flush, and refresh curriculum data for different deployment scenarios.

## Table of Contents

1. [Overview](#overview)
2. [Configuration Files](#configuration-files)
3. [Database Schema](#database-schema)
4. [Seeding Options](#seeding-options)
5. [Using the Seed Script](#using-the-seed-script)
6. [Manual SQL Seeding](#manual-sql-seeding)
7. [Deployment Scenarios](#deployment-scenarios)
8. [Troubleshooting](#troubleshooting)

## Overview

The Student Onboarding system supports two types of curriculum programs:

### Accredited Academic Programs
Traditional university/college programs that are accredited and lead to recognized degrees:
- Bachelor's degrees (BS, BA, BBA)
- Master's degrees (MS, MBA)
- Professional certificates (Data Analytics, Project Management)

### Vocational CDL Programs
Non-accredited vocational training programs for Commercial Driver's Licenses:
- **Class A CDL**: Tractor-trailers and combination vehicles
- **Class B CDL**: Straight trucks, buses, dump trucks
- **Endorsements**: Doubles/Triples (T), Tanker (N), Hazmat (H), Passenger (P), School Bus (S), Air Brakes

## Configuration Files

The curriculum data is defined in JSON configuration files located in `/config/`:

| File | Description |
|------|-------------|
| `curriculum_config.json` | Combined configuration with all programs |
| `curriculum_accredited.json` | Accredited academic programs only |
| `curriculum_vocational.json` | Vocational CDL programs only |

### Configuration Structure

```json
{
  "departments": [
    {
      "id": "computer_science",
      "name": "Computer Science",
      "code": "CS",
      "dean": "Dr. Jane Smith",
      "contactEmail": "cs@university.edu",
      "curriculumIds": ["cs_bs", "cs_ms", "cert_da"]
    }
  ],
  "curriculums": [
    {
      "id": "cs_bs",
      "name": "Bachelor of Science in Computer Science",
      "description": "...",
      "department": "computer_science",
      "degreeType": "bachelor",
      "creditHours": 120,
      "durationSemesters": 8,
      "requiredForms": ["personal_info", "emergency_contact", "academic_history", "consent"],
      "prerequisites": [],
      "isActive": true,
      "isOnline": false,
      "isAccredited": true,
      "programType": "academic"
    }
  ]
}
```

### CDL-Specific Fields

Vocational CDL programs include additional fields:

```json
{
  "id": "cdl_class_a",
  "programType": "vocational",
  "isAccredited": false,
  "cdlClass": "A",
  "endorsements": [],
  "trainingHours": {
    "classroom": 40,
    "range": 60,
    "road": 60
  }
}
```

## Database Schema

The curriculum is stored in PostgreSQL with the following tables:

### `department` Table
| Column | Type | Description |
|--------|------|-------------|
| id | SERIAL | Primary key |
| code | VARCHAR(20) | Department code (e.g., "CS", "CDL") |
| name | VARCHAR(200) | Full department name |
| dean | VARCHAR(200) | Department dean/director |
| contact_email | VARCHAR(200) | Contact email |

### `curriculum` Table
| Column | Type | Description |
|--------|------|-------------|
| id | SERIAL | Primary key |
| code | VARCHAR(50) | Unique curriculum code |
| name | VARCHAR(200) | Program name |
| description | TEXT | Full description |
| department_id | INTEGER | FK to department |
| degree_type | VARCHAR(50) | bachelor, master, certificate |
| credit_hours | INTEGER | Total credit/training hours |
| duration_semesters | INTEGER | Program duration |
| is_active | BOOLEAN | Active enrollment status |
| is_online | BOOLEAN | Online availability |
| is_accredited | BOOLEAN | Accreditation status |
| program_type | VARCHAR(50) | academic or vocational |
| cdl_class | VARCHAR(10) | CDL class (A or B) |
| endorsements | TEXT | JSON array of endorsements |
| training_hours_classroom | INTEGER | Classroom hours (CDL) |
| training_hours_range | INTEGER | Range/yard hours (CDL) |
| training_hours_road | INTEGER | Road hours (CDL) |

### `curriculum_prerequisite` Table
| Column | Type | Description |
|--------|------|-------------|
| id | SERIAL | Primary key |
| curriculum_id | INTEGER | FK to curriculum |
| prerequisite_description | VARCHAR(500) | Prerequisite requirement |

### `curriculum_form_requirement` Table
| Column | Type | Description |
|--------|------|-------------|
| id | SERIAL | Primary key |
| curriculum_id | INTEGER | FK to curriculum |
| form_type_id | INTEGER | FK to form_type |

## Seeding Options

### Migration Files

Located in `/database/migrations/`:

| File | Description |
|------|-------------|
| `004_extend_curriculum_schema.sql` | Adds new fields for vocational programs |
| `005_seed_accredited_curriculum.sql` | Seeds accredited academic programs |
| `005_seed_vocational_curriculum.sql` | Seeds vocational CDL programs |
| `006_flush_curriculum.sql` | Clears all curriculum data |

## Using the Seed Script

The easiest way to manage curriculum data is using the provided shell script:

```bash
# Make executable (first time only)
chmod +x scripts/seed_curriculum.sh

# View help
./scripts/seed_curriculum.sh --help
```

### Common Commands

```bash
# Seed BOTH accredited and vocational programs (default)
./scripts/seed_curriculum.sh

# Seed ONLY accredited academic programs (traditional university)
./scripts/seed_curriculum.sh --accredited

# Seed ONLY vocational CDL programs (driving school)
./scripts/seed_curriculum.sh --vocational

# Flush all curriculum data without re-seeding
./scripts/seed_curriculum.sh --flush-only

# Add programs without flushing existing data (upsert mode)
./scripts/seed_curriculum.sh --no-flush --vocational
```

### Environment Variables

Configure database connection via environment variables:

```bash
export DB_HOST=localhost
export DB_PORT=5432
export DB_NAME=student_intake
export DB_USER=postgres
export PGPASSWORD=your_password

./scripts/seed_curriculum.sh
```

## Manual SQL Seeding

If you prefer to run SQL directly:

### Flush Existing Data

```bash
psql -h localhost -U postgres -d student_intake -f database/migrations/006_flush_curriculum.sql
```

### Seed Accredited Programs

```bash
psql -h localhost -U postgres -d student_intake -f database/migrations/005_seed_accredited_curriculum.sql
```

### Seed Vocational Programs

```bash
psql -h localhost -U postgres -d student_intake -f database/migrations/005_seed_vocational_curriculum.sql
```

### Seed Both

```bash
psql -h localhost -U postgres -d student_intake -f database/migrations/006_flush_curriculum.sql
psql -h localhost -U postgres -d student_intake -f database/migrations/005_seed_accredited_curriculum.sql
psql -h localhost -U postgres -d student_intake -f database/migrations/005_seed_vocational_curriculum.sql
```

## Deployment Scenarios

### Scenario 1: Traditional University

For a traditional academic institution with degree programs:

```bash
./scripts/seed_curriculum.sh --accredited
```

**Programs included:**
- Computer Science (BS, MS)
- Business Administration (BBA, MBA)
- Engineering (BSEE, BSME)
- Nursing (BSN)
- Arts & Sciences (BA Psychology)
- Professional Certificates (Data Analytics, Project Management)

### Scenario 2: CDL Driving School

For a vocational driving school:

```bash
./scripts/seed_curriculum.sh --vocational
```

**Programs included:**
- Class A CDL Training (base program)
- Class A Endorsements: Doubles/Triples (T), Tanker (N), Hazmat (H), Air Brakes
- Class B CDL Training (base program)
- Class B Endorsements: Passenger (P), School Bus (S), Air Brakes

### Scenario 3: Community College / Combined Institution

For institutions offering both academic and vocational programs:

```bash
./scripts/seed_curriculum.sh --both
# or simply:
./scripts/seed_curriculum.sh
```

### Scenario 4: Adding New Programs to Existing Data

To add vocational programs to an existing accredited setup without losing data:

```bash
./scripts/seed_curriculum.sh --no-flush --vocational
```

## CDL Program Structure

### Class A CDL Hierarchy

```
Class A CDL Training Program (base)
├── Doubles/Triples Endorsement (T) [requires: base]
├── Tanker Endorsement (N) [requires: base]
├── Hazmat Endorsement (H) [requires: base]
└── Air Brakes Training [standalone]
```

### Class B CDL Hierarchy

```
Class B CDL Training Program (base)
├── Passenger Endorsement (P) [requires: base]
│   └── School Bus Endorsement (S) [requires: P + base]
└── Air Brakes Training [standalone]
```

### Training Hours by Program

| Program | Classroom | Range | Road | Total |
|---------|-----------|-------|------|-------|
| Class A Base | 40 | 60 | 60 | 160 |
| Class A Doubles/Triples | 8 | 16 | 16 | 40 |
| Class A Tanker | 12 | 14 | 14 | 40 |
| Class A Hazmat | 24 | 4 | 4 | 32 |
| Class A Air Brakes | 8 | 8 | 8 | 24 |
| Class B Base | 30 | 45 | 45 | 120 |
| Class B Passenger | 12 | 14 | 14 | 40 |
| Class B School Bus | 16 | 12 | 12 | 40 |
| Class B Air Brakes | 8 | 8 | 8 | 24 |

## Troubleshooting

### Connection Issues

```bash
# Test database connection
psql -h localhost -U postgres -d student_intake -c "SELECT 1"

# Check if database exists
psql -h localhost -U postgres -l | grep student_intake
```

### Schema Issues

If you get errors about missing columns:

```bash
# Run schema migration first
psql -h localhost -U postgres -d student_intake -f database/migrations/004_extend_curriculum_schema.sql
```

### Foreign Key Constraints

The flush script handles foreign key constraints in the correct order:
1. Clear `curriculum_form_requirement`
2. Clear `curriculum_prerequisite`
3. Nullify `student.curriculum_id` references
4. Clear `curriculum`
5. Clear `department`

### Verify Seeding Results

```sql
-- Check department counts
SELECT code, name FROM department ORDER BY name;

-- Check curriculum by type
SELECT
    program_type,
    is_accredited,
    COUNT(*) as count
FROM curriculum
GROUP BY program_type, is_accredited;

-- Check CDL programs
SELECT code, name, cdl_class, endorsements
FROM curriculum
WHERE program_type = 'vocational'
ORDER BY cdl_class, code;
```

## API Integration

The curriculum data seeded via SQL is available through the API at:

```
GET /api/Curriculum
GET /api/Curriculum/{id}
GET /api/Department
GET /api/Department/{id}
```

Filter by program type:

```
GET /api/Curriculum?filter[program_type]=vocational
GET /api/Curriculum?filter[is_accredited]=true
GET /api/Curriculum?filter[cdl_class]=A
```

## Customization

To add custom curriculum programs:

1. Create a new migration file (e.g., `007_seed_custom_curriculum.sql`)
2. Follow the INSERT pattern from existing seed files
3. Use `ON CONFLICT DO UPDATE` for upsert behavior
4. Run with `--no-flush` option to preserve existing data

Example:

```sql
INSERT INTO curriculum (code, name, description, department_id, degree_type, ...)
VALUES ('custom_program', 'Custom Program', '...', ...)
ON CONFLICT (code) DO UPDATE SET
    name = EXCLUDED.name,
    description = EXCLUDED.description,
    ...;
```

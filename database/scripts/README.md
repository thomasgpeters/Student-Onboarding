# Program Mode Migration Guide

This guide explains how to switch between **Accredited** (university/college) and **Vocational** (trade school/CDL) program configurations.

## Overview

The Student Onboarding system supports two distinct program modes:

| Mode | Institution Type | Departments | Degree Types | Example Programs |
|------|-----------------|-------------|--------------|------------------|
| **Accredited** | University, College | Computer Science, Business, Engineering, Nursing, Arts & Sciences | Associate, Bachelor, Master, Doctoral, Certificate | BS in Computer Science, MBA, BSN |
| **Vocational** | Trade School, CDL Training | Professional Driving, Automotive, Electrical, Welding, HVAC, etc. | Certificate | Class A CDL, Tanker Endorsement, Air Brakes |

## Quick Start

### Switch to Accredited Mode (University Programs)

```bash
psql -U your_user -d your_database -f switch_to_accredited.sql
```

### Switch to Vocational Mode (Trade School / CDL Programs)

```bash
psql -U your_user -d your_database -f switch_to_vocational.sql
```

## What Gets Changed

When switching modes, the following data is affected:

### Deleted and Replaced
- **Departments** - All departments are cleared and replaced with mode-specific departments
- **Curriculum** - All programs are cleared and replaced
- **Curriculum Form Requirements** - Junction table entries linking programs to required forms
- **Curriculum Prerequisites** - Program prerequisite relationships

### Modified
- **Student Records** - `curriculum_id` is set to NULL (students remain but lose program association)
- **Institution Settings** - `institution_type`, `program_mode`, `degree_types_enabled` are updated

### Preserved
- **Students** - All student records are preserved (only curriculum_id is nullified)
- **Form Submissions** - All submitted forms are preserved
- **Form Types** - Form definitions remain unchanged
- **Admin Users** - All administrator accounts are preserved

## Detailed Mode Comparison

### Accredited Mode Departments

| Code | Department Name | Contact |
|------|----------------|---------|
| CS | Computer Science | cs@university.edu |
| BUS | Business Administration | business@university.edu |
| ENG | Engineering | engineering@university.edu |
| NUR | Nursing | nursing@university.edu |
| A&S | Arts and Sciences | artsci@university.edu |

### Vocational Mode Departments

| Code | Department Name | Contact |
|------|----------------|---------|
| CDL | Professional Driving | driving@school.edu |
| AUTO | Automotive Technology | automotive@school.edu |
| FOOD | Food Services | culinary@school.edu |
| TRADE | Skilled Trades | trades@school.edu |
| ELECT | Electrical Technology | electrical@school.edu |
| WELD | Welding Technology | welding@school.edu |
| HVAC | HVAC Technology | hvac@school.edu |
| MED | Medical Services | medical@school.edu |

## Duration Intervals

Programs support different duration intervals:

| Mode | Typical Interval | Example |
|------|-----------------|---------|
| Accredited | Semester | "8 Semesters" for a 4-year degree |
| Vocational | Week, Day | "4 Weeks" for CDL training |

The `duration_interval` field can be: `semester`, `month`, `week`, or `day`

## Backup Before Switching

**Always backup your database before switching modes!**

```bash
# PostgreSQL backup
pg_dump -U your_user -d your_database > backup_before_switch.sql

# Then switch
psql -U your_user -d your_database -f switch_to_vocational.sql
```

## Restoring After Switch

If you need to restore after switching:

```bash
# Drop and recreate database
dropdb your_database
createdb your_database

# Restore from backup
psql -U your_user -d your_database < backup_before_switch.sql
```

## Manual Migration Using Individual Scripts

If you prefer more control, use the individual migration scripts:

```bash
# Step 1: Flush all curriculum data
psql -f ../migrations/006_flush_curriculum.sql

# Step 2: Run the desired seed script
psql -f ../migrations/005_seed_accredited_curriculum.sql
# OR
psql -f ../migrations/005_seed_vocational_curriculum.sql

# Step 3: Update institution settings (optional)
psql -f ../migrations/009_add_institution_type.sql
```

## Adding Custom Programs

After switching modes, you can add custom programs through:

1. **Admin UI** - Use the Curriculum Management section
2. **SQL** - Insert directly into the curriculum table
3. **API** - POST to `/api/Curriculum` endpoint

## Troubleshooting

### Students Lost Program Assignment
After switching, students' `curriculum_id` is set to NULL. Reassign them via:
- Admin UI: Edit student and select new program
- SQL: `UPDATE student SET curriculum_id = X WHERE id = Y`

### Foreign Key Violations
If you get foreign key errors, ensure you run scripts in order:
1. Clear form requirements first
2. Clear prerequisites
3. Nullify student references
4. Clear curriculum
5. Clear departments

The switch scripts handle this automatically.

### Missing Duration Interval
Run migration 008 if you see duration interval issues:
```bash
psql -f ../migrations/008_add_duration_interval.sql
```

## File Reference

| File | Purpose |
|------|---------|
| `switch_to_accredited.sql` | Complete switch to university mode |
| `switch_to_vocational.sql` | Complete switch to trade school mode |
| `../migrations/005_seed_accredited_curriculum.sql` | Accredited seed data only |
| `../migrations/005_seed_vocational_curriculum.sql` | Vocational seed data only |
| `../migrations/006_flush_curriculum.sql` | Clear all curriculum data |
| `../migrations/009_add_institution_type.sql` | Add institution type settings |

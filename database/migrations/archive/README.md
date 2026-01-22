# Archived Migration Files

This directory contains historical migration files that have been consolidated into the main `schema.sql` file.

## Why Archived?

The individual migration files were used during initial development to incrementally build the database schema. Now that the schema is stable, all changes have been consolidated into:

- `database/schema.sql` - Complete database schema with all tables, indexes, and triggers
- `database/install.sql` - Single installation script that sets up the complete database

## Files in Archive

| File | Description |
|------|-------------|
| `001_add_student_fields.sql` | Extended student table with additional fields |
| `002_add_admin_tables.sql` | Admin user and session tables |
| `003_seed_sample_students.sql` | Sample student data for testing |
| `004_extend_curriculum_schema.sql` | Curriculum table extensions for vocational support |
| `005_seed_accredited_curriculum.sql` | Seed data for accredited programs |
| `005_seed_vocational_curriculum.sql` | Seed data for vocational programs |
| `006_flush_curriculum.sql` | Script to clear curriculum data |
| `007_institution_settings.sql` | Institution settings table |
| `008_add_duration_interval.sql` | Duration interval field for curriculum |
| `009_add_institution_type.sql` | Institution type settings |
| `010_academic_history_compound_key.sql` | Academic history compound primary key |
| `011_emergency_contact_compound_key.sql` | Emergency contact compound primary key |
| `012_financial_aid_missing_columns.sql` | Additional financial aid columns |

## Do Not Use

These files are kept for historical reference only. For new installations, use:

```bash
psql -U postgres -d student_onboarding -f database/install.sql
```

## For Future Schema Changes

Future schema changes should be made directly to `database/schema.sql` and documented in `DATABASE.md`.

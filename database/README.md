# Student Onboarding Database

This directory contains all database-related files for the Student Onboarding System.

## User Management Architecture

The system uses a **unified user management architecture** where all user credentials are stored in a single `app_user` table:

```
┌──────────────┐     ┌─────────────┐     ┌─────────────┐
│   app_user   │────>│  user_role  │     │   student   │
│ (credentials)│     │ (role type) │     │  (profile)  │
└──────────────┘     └─────────────┘     └─────────────┘
       │                                        │
       │                                        │
       │                                 app_user_id
       │                                        │
       │             ┌─────────────┐            │
       └────────────>│ admin_user  │<───────────┘
                     │  (profile)  │
                     └─────────────┘
```

### Key Tables

| Table | Purpose |
|-------|---------|
| `app_user` | **Single source of truth** for all user credentials (email, password_hash) |
| `user_role` | Role assignments (student, instructor, admin, super_admin, super_user) |
| `student` | Student profile data, linked via `app_user_id` |
| `admin_user` | Admin/instructor profile data, linked via `app_user_id` |

### Login Flow

1. User submits credentials (email/password)
2. System validates against `app_user` table
3. System checks `user_role` table for assigned roles
4. System loads appropriate profile from `student` or `admin_user` based on role
5. User is redirected to the appropriate dashboard

## Database Setup

### Prerequisites

- PostgreSQL 12 or higher
- psql command-line tool (or a GUI like pgAdmin/DBeaver)

### Fresh Installation

Run these commands in order:

```bash
# 1. Create the database
createdb student_onboarding

# 2. Create schema (tables, indexes, triggers)
psql -U postgres -d student_onboarding -f database/schema.sql

# 3. Install base configuration (form types, settings, default admin)
psql -U postgres -d student_onboarding -f database/install.sql

# 4. Set curriculum mode (choose one):
# For CDL/vocational school:
psql -U postgres -d student_onboarding -f database/scripts/switch_to_vocational.sql
# For university/college:
psql -U postgres -d student_onboarding -f database/scripts/switch_to_accredited.sql

# 5. (Optional) Load seed data for testing
psql -U postgres -d student_onboarding -f database/seed_data.sql

# 6. (Optional) Add instructor features (migration 014)
psql -U postgres -d student_onboarding -f database/migrations/014_add_instructor_feature.sql
```

### Re-seeding the Database

If you need to reset and re-seed the database:

```bash
# Drop and recreate the database
dropdb student_onboarding
createdb student_onboarding

# Run the full installation sequence
psql -U postgres -d student_onboarding -f database/schema.sql
psql -U postgres -d student_onboarding -f database/install.sql
psql -U postgres -d student_onboarding -f database/scripts/switch_to_vocational.sql
psql -U postgres -d student_onboarding -f database/seed_data.sql
```

## Seed Data Details

The `seed_data.sql` script creates test users with properly linked records across all tables.

### How Seed Data Works

For each user, the script:

1. **Creates `app_user` record** - stores email, password_hash, name
2. **Creates `user_role` entry** - assigns role (student/instructor/admin)
3. **Creates profile record** - `student` or `admin_user` with `app_user_id` link

This ensures the `app_user.id` is properly linked to profile tables, preventing the "mismatched ID" issues.

### Test Credentials

All seed users use the same password: `Password123!`

#### Admins
| Email | Name | Role |
|-------|------|------|
| admin@institution.edu | System Administrator | super_admin |
| director@cdlschool.edu | Margaret Reynolds | admin |
| manager@cdlschool.edu | Thomas Garcia | admin |
| admissions@cdlschool.edu | Jennifer Martinez | admin |

#### Instructors
| Email | Name | Specialty |
|-------|------|-----------|
| j.williams@cdlschool.edu | James Williams | Class A, Senior + Examiner |
| m.johnson@cdlschool.edu | Michael Johnson | Class A |
| s.davis@cdlschool.edu | Sarah Davis | Class B |
| r.thompson@cdlschool.edu | Robert Thompson | Examiner Only |

#### Students
| Email | Name | Program |
|-------|------|---------|
| john.smith@email.com | John Smith | Class A (Veteran) |
| maria.garcia@email.com | Maria Garcia | Class A |
| david.lee@email.com | David Lee | Class A |
| lisa.chen@email.com | Lisa Chen | Class B |
| kevin.brown@email.com | Kevin Brown | Class B |
| amanda.wilson@email.com | Amanda Wilson | Class A |
| marcus.taylor@email.com | Marcus Taylor | Class A |
| carlos.rodriguez@email.com | Carlos Rodriguez | Class A (Perm Resident) |

## Troubleshooting

### "Missing Parent" Error on Login

If you see a "Missing Parent" constraint error when a student tries to login, the `student` record is not linked to an `app_user` record.

**Fix:** Update the student record to set the `app_user_id`:

```sql
-- Find the app_user.id for the email
SELECT id FROM app_user WHERE email = 'john.smith@email.com';
-- Returns: 17

-- Update the student record
UPDATE student SET app_user_id = 17 WHERE email = 'john.smith@email.com';
```

Or re-run `seed_data.sql` which will automatically fix unlinked records.

### Student/Admin Profile Not Found After Login

If login succeeds but the profile isn't found, check that:

1. The `app_user_id` is set in the `student` or `admin_user` table
2. The `user_role` entry exists for the user

```sql
-- Check linkage for a user
SELECT
    au.id as app_user_id,
    au.email,
    ur.role,
    s.id as student_id,
    s.app_user_id as student_link,
    a.id as admin_id,
    a.app_user_id as admin_link
FROM app_user au
LEFT JOIN user_role ur ON ur.app_user_id = au.id
LEFT JOIN student s ON s.email = au.email
LEFT JOIN admin_user a ON a.email = au.email
WHERE au.email = 'john.smith@email.com';
```

### Idempotent Seed Data

The seed script is designed to be **idempotent** - it can be run multiple times safely:

- Uses `NOT EXISTS` checks before inserting new records
- Uses `ON CONFLICT DO NOTHING` where applicable
- Updates `app_user_id` on existing records if they were created without proper linkage

## Directory Structure

```
database/
├── README.md              # This file
├── schema.sql             # Database schema (tables, indexes, triggers)
├── install.sql            # Base installation (form types, settings, default admin)
├── seed_data.sql          # Test data for development
├── migrations/            # Database migrations
│   ├── 001_*.sql
│   ├── ...
│   └── 014_add_instructor_feature.sql
└── scripts/               # Utility scripts
    ├── switch_to_accredited.sql
    ├── switch_to_vocational.sql
    └── README.md
```

## Related Documentation

- [API Documentation](../docs/API.md) - REST API endpoints
- [Authentication Flow](../docs/AUTH.md) - Login and session management
- [Instructor Feature](./migrations/archive/README.md) - Instructor management migration

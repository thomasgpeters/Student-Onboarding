#!/bin/bash
# ==============================================================================
# Curriculum Seeding Script
# ==============================================================================
# This script handles curriculum data seeding for the Student Onboarding system.
# It supports seeding accredited programs, vocational programs, or both.
#
# Usage:
#   ./seed_curriculum.sh [options]
#
# Options:
#   --accredited    Seed accredited academic programs only
#   --vocational    Seed vocational CDL programs only
#   --both          Seed both accredited and vocational programs (default)
#   --flush-only    Only flush existing data without re-seeding
#   --no-flush      Seed without flushing (upsert mode)
#   --help          Show this help message
#
# Environment Variables:
#   DB_HOST         Database host (default: localhost)
#   DB_PORT         Database port (default: 5432)
#   DB_NAME         Database name (default: student_intake)
#   DB_USER         Database user (default: postgres)
#   PGPASSWORD      Database password (read from environment)
#
# Examples:
#   # Seed both accredited and vocational programs (default)
#   ./seed_curriculum.sh
#
#   # Seed only accredited programs for a traditional university
#   ./seed_curriculum.sh --accredited
#
#   # Seed only CDL vocational programs for a driving school
#   ./seed_curriculum.sh --vocational
#
#   # Just flush data without re-seeding
#   ./seed_curriculum.sh --flush-only
#
#   # Add new programs without removing existing data
#   ./seed_curriculum.sh --no-flush --vocational
# ==============================================================================

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Default configuration
DB_HOST="${DB_HOST:-localhost}"
DB_PORT="${DB_PORT:-5432}"
DB_NAME="${DB_NAME:-student_intake}"
DB_USER="${DB_USER:-postgres}"

# Script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
MIGRATIONS_DIR="${SCRIPT_DIR}/../database/migrations"

# Default options
SEED_ACCREDITED=false
SEED_VOCATIONAL=false
FLUSH_FIRST=true
FLUSH_ONLY=false

# Parse command line arguments
parse_args() {
    if [ $# -eq 0 ]; then
        # Default: seed both
        SEED_ACCREDITED=true
        SEED_VOCATIONAL=true
        return
    fi

    while [[ $# -gt 0 ]]; do
        case $1 in
            --accredited)
                SEED_ACCREDITED=true
                shift
                ;;
            --vocational)
                SEED_VOCATIONAL=true
                shift
                ;;
            --both)
                SEED_ACCREDITED=true
                SEED_VOCATIONAL=true
                shift
                ;;
            --flush-only)
                FLUSH_ONLY=true
                shift
                ;;
            --no-flush)
                FLUSH_FIRST=false
                shift
                ;;
            --help|-h)
                show_help
                exit 0
                ;;
            *)
                echo -e "${RED}Unknown option: $1${NC}"
                show_help
                exit 1
                ;;
        esac
    done

    # If no seed options specified but not flush-only, default to both
    if [ "$FLUSH_ONLY" = false ] && [ "$SEED_ACCREDITED" = false ] && [ "$SEED_VOCATIONAL" = false ]; then
        SEED_ACCREDITED=true
        SEED_VOCATIONAL=true
    fi
}

show_help() {
    head -50 "$0" | tail -45
}

# Print status message
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check prerequisites
check_prerequisites() {
    print_status "Checking prerequisites..."

    if ! command -v psql &> /dev/null; then
        print_error "psql command not found. Please install PostgreSQL client."
        exit 1
    fi

    # Check if we can connect to the database
    if ! psql -h "$DB_HOST" -p "$DB_PORT" -U "$DB_USER" -d "$DB_NAME" -c "SELECT 1" &> /dev/null; then
        print_error "Cannot connect to database. Please check connection settings."
        print_status "Connection details:"
        print_status "  Host: $DB_HOST"
        print_status "  Port: $DB_PORT"
        print_status "  Database: $DB_NAME"
        print_status "  User: $DB_USER"
        exit 1
    fi

    print_success "Database connection verified"
}

# Run a SQL file
run_sql_file() {
    local file="$1"
    local description="$2"

    if [ ! -f "$file" ]; then
        print_error "SQL file not found: $file"
        exit 1
    fi

    print_status "Running: $description"
    psql -h "$DB_HOST" -p "$DB_PORT" -U "$DB_USER" -d "$DB_NAME" -f "$file" -q
    print_success "Completed: $description"
}

# Check and run schema migration
ensure_schema() {
    print_status "Checking curriculum schema..."

    # Check if new columns exist
    local has_columns=$(psql -h "$DB_HOST" -p "$DB_PORT" -U "$DB_USER" -d "$DB_NAME" -t -c \
        "SELECT COUNT(*) FROM information_schema.columns
         WHERE table_name = 'curriculum' AND column_name = 'is_accredited'")

    if [ "$(echo $has_columns | tr -d ' ')" = "0" ]; then
        print_warning "Schema needs updating. Running migration 004..."
        run_sql_file "${MIGRATIONS_DIR}/004_extend_curriculum_schema.sql" "Schema extension migration"
    else
        print_success "Schema is up to date"
    fi
}

# Main execution
main() {
    echo ""
    echo "=============================================="
    echo "  Student Onboarding - Curriculum Seeding"
    echo "=============================================="
    echo ""

    parse_args "$@"
    check_prerequisites
    ensure_schema

    echo ""
    print_status "Seeding configuration:"
    print_status "  Flush first: $FLUSH_FIRST"
    print_status "  Flush only: $FLUSH_ONLY"
    print_status "  Seed accredited: $SEED_ACCREDITED"
    print_status "  Seed vocational: $SEED_VOCATIONAL"
    echo ""

    # Flush if requested
    if [ "$FLUSH_FIRST" = true ]; then
        run_sql_file "${MIGRATIONS_DIR}/006_flush_curriculum.sql" "Flushing existing curriculum data"
    fi

    # Exit if flush-only
    if [ "$FLUSH_ONLY" = true ]; then
        print_success "Flush completed. No seeding performed."
        exit 0
    fi

    # Seed accredited programs
    if [ "$SEED_ACCREDITED" = true ]; then
        run_sql_file "${MIGRATIONS_DIR}/005_seed_accredited_curriculum.sql" "Seeding accredited academic programs"
    fi

    # Seed vocational programs
    if [ "$SEED_VOCATIONAL" = true ]; then
        run_sql_file "${MIGRATIONS_DIR}/005_seed_vocational_curriculum.sql" "Seeding vocational CDL programs"
    fi

    echo ""
    echo "=============================================="
    print_success "Curriculum seeding completed successfully!"
    echo "=============================================="

    # Show summary
    echo ""
    print_status "Summary of seeded data:"
    psql -h "$DB_HOST" -p "$DB_PORT" -U "$DB_USER" -d "$DB_NAME" -c \
        "SELECT
            d.name as department,
            COUNT(c.id) as programs,
            SUM(CASE WHEN c.is_accredited THEN 1 ELSE 0 END) as accredited,
            SUM(CASE WHEN NOT c.is_accredited THEN 1 ELSE 0 END) as vocational
         FROM department d
         LEFT JOIN curriculum c ON c.department_id = d.id
         GROUP BY d.id, d.name
         ORDER BY d.name"
    echo ""
}

# Run main
main "$@"

#!/bin/bash
# =============================================================================
# Student Onboarding Application - Container Entrypoint
# =============================================================================

set -e

# Display startup banner
echo "=============================================="
echo "  Student Onboarding Application"
echo "=============================================="
echo "  Log Level: ${LOG_LEVEL:-INFO}"
echo "  HTTP Port: ${HTTP_PORT:-8080}"
echo "  HTTP Address: ${HTTP_ADDRESS:-0.0.0.0}"
echo "=============================================="

# Export environment variables for the application
export LOG_LEVEL="${LOG_LEVEL:-INFO}"

# Build command arguments
ARGS="$@"

# Override port if HTTP_PORT is set
if [ -n "$HTTP_PORT" ]; then
    # Remove any existing --http-port from args and add our own
    ARGS=$(echo "$ARGS" | sed 's/--http-port[[:space:]]*[0-9]*//')
    ARGS="$ARGS --http-port $HTTP_PORT"
fi

# Override address if HTTP_ADDRESS is set
if [ -n "$HTTP_ADDRESS" ]; then
    # Remove any existing --http-address from args and add our own
    ARGS=$(echo "$ARGS" | sed 's/--http-address[[:space:]]*[^[:space:]]*//')
    ARGS="$ARGS --http-address $HTTP_ADDRESS"
fi

# Execute the application
exec ./student_intake $ARGS

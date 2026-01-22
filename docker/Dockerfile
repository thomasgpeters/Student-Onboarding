# =============================================================================
# Student Onboarding Application - Docker Build
# =============================================================================
# Multi-stage build for optimized image size
# Base: Ubuntu 20.04 LTS
# =============================================================================

# -----------------------------------------------------------------------------
# Stage 1: Builder
# -----------------------------------------------------------------------------
FROM ubuntu:20.04 AS builder

# Prevent interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install build dependencies
RUN apt-get update && apt-get install -y \
    # Build tools
    cmake \
    g++ \
    make \
    pkg-config \
    # Wt web framework
    libwt-dev \
    libwthttp-dev \
    # PDF generation (libharu)
    libhpdf-dev \
    # JSON library
    nlohmann-json3-dev \
    # HTTP client
    libcurl4-openssl-dev \
    # SSL support
    libssl-dev \
    # Boost (required by Wt)
    libboost-all-dev \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy source code
COPY CMakeLists.txt .
COPY src/ src/
COPY config/ config/
COPY resources/ resources/

# Create build directory and compile
RUN mkdir -p build && cd build \
    && cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_CXX_FLAGS="-O2" \
    && make -j$(nproc)

# -----------------------------------------------------------------------------
# Stage 2: Runtime
# -----------------------------------------------------------------------------
FROM ubuntu:20.04 AS runtime

# Prevent interactive prompts
ENV DEBIAN_FRONTEND=noninteractive

# Install runtime dependencies only
RUN apt-get update && apt-get install -y --no-install-recommends \
    # Wt runtime libraries
    libwt42 \
    libwthttp42 \
    # PDF generation runtime
    libhpdf-2.3.0 \
    # HTTP client runtime
    libcurl4 \
    # SSL runtime
    libssl1.1 \
    # Boost runtime (required by Wt)
    libboost-filesystem1.71.0 \
    libboost-thread1.71.0 \
    libboost-program-options1.71.0 \
    libboost-system1.71.0 \
    # Cleanup
    && rm -rf /var/lib/apt/lists/* \
    && apt-get clean

# Create non-root user for security
RUN useradd -m -s /bin/bash appuser

# Set working directory
WORKDIR /app

# Copy compiled binary from builder
COPY --from=builder /app/build/student_intake .

# Copy runtime resources
COPY --from=builder /app/config/ config/
COPY --from=builder /app/resources/ resources/

# Copy entrypoint script
COPY docker/entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh

# Change ownership to non-root user
RUN chown -R appuser:appuser /app

# Switch to non-root user
USER appuser

# Environment variables
ENV LOG_LEVEL=INFO
ENV HTTP_PORT=8080
ENV HTTP_ADDRESS=0.0.0.0

# Expose the application port
EXPOSE 8080

# Health check
HEALTHCHECK --interval=30s --timeout=10s --start-period=5s --retries=3 \
    CMD curl -f http://localhost:8080/ || exit 1

# Set entrypoint
ENTRYPOINT ["/entrypoint.sh"]

# Default command arguments
CMD ["--docroot", "resources", "--http-address", "0.0.0.0", "--http-port", "8080"]

```
╔═══════════════════════════════════════════════════════════════╗
║         STUDENT ONBOARD - Docker Deployment Guide             ║
╚═══════════════════════════════════════════════════════════════╝
```

# Docker Deployment Guide

This guide explains how to build and deploy the Student Onboarding application using Docker.

## Prerequisites

- Docker 20.10 or later
- Docker Compose 2.0 or later (optional, for easier deployment)
- At least 2GB of free disk space for the build

## Quick Start

### Using Docker Compose (Recommended)

```bash
# Navigate to the docker directory
cd docker

# Build and start the container
docker-compose up -d

# View logs
docker-compose logs -f

# Stop the container
docker-compose down
```

### Using Docker Directly

```bash
# Build the image from repository root
docker build -f docker/Dockerfile -t student-onboarding:latest .

# Run the container
docker run -d \
  --name student-onboarding \
  -p 8080:8080 \
  -e LOG_LEVEL=INFO \
  student-onboarding:latest
```

## Accessing the Application

Once running, access the application at:

| Portal | URL |
|--------|-----|
| Student Portal | http://localhost:8080/ |
| Admin Portal | http://localhost:8080/administration |

## Configuration

### Environment Variables

| Variable | Default | Description |
|----------|---------|-------------|
| `LOG_LEVEL` | `INFO` | Logging verbosity: `DEBUG`, `INFO`, `WARN`, `ERROR`, `NONE` |
| `HTTP_PORT` | `8080` | Port the application listens on |
| `HTTP_ADDRESS` | `0.0.0.0` | Address to bind to |
| `HOST_PORT` | `8080` | Host port mapping (docker-compose only) |

### Examples

```bash
# Run with debug logging
docker run -d -p 8080:8080 -e LOG_LEVEL=DEBUG student-onboarding:latest

# Run on a different port
docker run -d -p 3000:3000 -e HTTP_PORT=3000 student-onboarding:latest

# Using docker-compose with custom settings
LOG_LEVEL=DEBUG HOST_PORT=3000 docker-compose up -d
```

## Build Details

### Multi-Stage Build

The Dockerfile uses a multi-stage build for optimized image size:

| Stage | Base Image | Purpose | Size |
|-------|------------|---------|------|
| `builder` | Ubuntu 20.04 | Compile application | ~1.5GB |
| `runtime` | Ubuntu 20.04 | Run application | ~200MB |

### Dependencies

#### Build Dependencies (Stage 1)
- `cmake`, `g++`, `make` - Build tools
- `libwt-dev`, `libwthttp-dev` - Wt web framework
- `libhpdf-dev` - PDF generation (libharu)
- `nlohmann-json3-dev` - JSON parsing
- `libcurl4-openssl-dev` - HTTP client
- `libboost-all-dev` - Boost libraries (Wt dependency)

#### Runtime Dependencies (Stage 2)
- `libwt42`, `libwthttp42` - Wt runtime
- `libhpdf-2.3.0` - libharu runtime
- `libcurl4` - cURL runtime
- `libboost-*1.71.0` - Boost runtime libraries

## Container Details

### Security

- Runs as non-root user (`appuser`)
- Minimal runtime image with only required dependencies
- No build tools in final image

### Health Check

The container includes a health check that:
- Runs every 30 seconds
- Times out after 10 seconds
- Allows 5 seconds for startup
- Retries 3 times before marking unhealthy

```bash
# Check container health
docker inspect --format='{{.State.Health.Status}}' student-onboarding
```

### Resource Limits (docker-compose)

Default resource limits in docker-compose.yml:
- CPU: 2.0 cores max, 0.5 cores reserved
- Memory: 1GB max, 256MB reserved

Adjust in `docker-compose.yml` under `deploy.resources`.

## Troubleshooting

### View Logs

```bash
# Docker
docker logs student-onboarding

# Docker Compose
docker-compose logs -f

# With timestamps
docker logs -t student-onboarding
```

### Enter Container Shell

```bash
# Docker
docker exec -it student-onboarding /bin/bash

# Docker Compose
docker-compose exec student-onboarding /bin/bash
```

### Common Issues

#### Port Already in Use

```bash
# Find what's using the port
lsof -i :8080

# Use a different port
docker run -p 3000:8080 student-onboarding:latest
```

#### Build Fails

```bash
# Clean build (no cache)
docker build --no-cache -f docker/Dockerfile -t student-onboarding:latest .

# Check available disk space
df -h
```

#### Container Exits Immediately

```bash
# Check logs for errors
docker logs student-onboarding

# Run interactively to see output
docker run -it student-onboarding:latest
```

### API Backend Connection

The application expects an API backend (ApiLogicServer) at `http://localhost:5656/api`. When running in Docker:

1. **Host Network Mode** (connects to host's localhost):
   ```bash
   docker run --network host student-onboarding:latest
   ```

2. **Docker Network** (if API is also in Docker):
   ```yaml
   # In docker-compose.yml, use service name as hostname
   environment:
     - API_URL=http://api-backend:5656/api
   ```

3. **External API** (configure in application):
   Update the API URL in the application configuration.

## Production Deployment

### Recommended Settings

```yaml
# docker-compose.prod.yml
version: '3.8'
services:
  student-onboarding:
    image: student-onboarding:latest
    restart: always
    environment:
      - LOG_LEVEL=WARN
    deploy:
      resources:
        limits:
          cpus: '4.0'
          memory: 2G
    logging:
      driver: "json-file"
      options:
        max-size: "50m"
        max-file: "5"
```

### Behind a Reverse Proxy (nginx)

```nginx
server {
    listen 80;
    server_name onboarding.example.com;

    location / {
        proxy_pass http://localhost:8080;
        proxy_http_version 1.1;
        proxy_set_header Upgrade $http_upgrade;
        proxy_set_header Connection "upgrade";
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto $scheme;
    }
}
```

### Cloud Platform Deployment

#### AWS ECS / Fargate

```bash
# Tag for ECR
docker tag student-onboarding:latest <account>.dkr.ecr.<region>.amazonaws.com/student-onboarding:latest

# Push to ECR
docker push <account>.dkr.ecr.<region>.amazonaws.com/student-onboarding:latest
```

#### Google Cloud Run

```bash
# Tag for GCR
docker tag student-onboarding:latest gcr.io/<project>/student-onboarding:latest

# Push to GCR
docker push gcr.io/<project>/student-onboarding:latest

# Deploy
gcloud run deploy student-onboarding --image gcr.io/<project>/student-onboarding:latest
```

#### Azure Container Instances

```bash
# Tag for ACR
docker tag student-onboarding:latest <registry>.azurecr.io/student-onboarding:latest

# Push to ACR
docker push <registry>.azurecr.io/student-onboarding:latest
```

## File Structure

```
docker/
├── Dockerfile           # Multi-stage build definition
├── docker-compose.yml   # Service orchestration
└── entrypoint.sh        # Container startup script

.dockerignore            # Build context exclusions
```

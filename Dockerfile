# Build stage
FROM ghcr.io/userver-framework/ubuntu-22.04-userver-pg-dev:latest AS builder

RUN apt-get update && apt-get install -y \
    libargon2-dev \
    && rm -rf /var/lib/apt/lists/*

ENV PRESETS="release"
ENV DESTDIR=/app
ENV PREFIX=/

WORKDIR /build
COPY . .

RUN make install-release

FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    --no-install-recommends \
    libargon2-1 \
    libldap-2.5-0 \
    ca-certificates \
    libcurl4 \
    libatomic1 \
    libfmt8 \
    libcctz2 \
    libpugixml1v5 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY --from=builder /app/ .

RUN mkdir -p secrets

ENTRYPOINT ["./bin/pawspective", "--config", "./etc/pawspective/static_config.yaml", "--config_vars", "./etc/pawspective/config_vars.yaml"]


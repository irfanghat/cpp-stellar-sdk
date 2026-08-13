FROM mcr.microsoft.com/devcontainers/cpp:3-debian13

COPY install-deps.sh /tmp/install-deps.sh

RUN apt-get update \
    && apt-get install -y --no-install-recommends git gh \
    && rm -rf /var/lib/apt/lists/*

RUN chmod +x /tmp/install-deps.sh \
    && /tmp/install-deps.sh \
    && rm -f /tmp/install-deps.sh

# ----------------------------------------------------------------
# Install additional vcpkg ports
# ----------------------------------------------------------------

# RUN su vscode -c "${VCPKG_ROOT}/vcpkg install <port-name>"

ENV CC=clang
ENV CXX=clang++

WORKDIR /workspaces
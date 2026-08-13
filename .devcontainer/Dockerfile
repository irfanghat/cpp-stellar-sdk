FROM mcr.microsoft.com/devcontainers/cpp:3-debian13

COPY install-deps.sh /tmp/install-deps.sh

RUN chmod +x /tmp/install-deps.sh \
    && /tmp/install-deps.sh \
    && rm -f /tmp/install-deps.sh \
    && rm -rf /var/lib/apt/lists/*

ENV CC=clang
ENV CXX=clang++

WORKDIR /workspaces
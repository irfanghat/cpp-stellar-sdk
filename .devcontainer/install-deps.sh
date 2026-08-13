#!/usr/bin/env bash

set -euo pipefail

CMAKE_VERSION="4.3.1"
CMAKE_DIR="cmake-${CMAKE_VERSION}-linux-x86_64"
CMAKE_TARBALL="${CMAKE_DIR}.tar.gz"
CMAKE_URL="https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/${CMAKE_TARBALL}"

log() {
    echo
    echo "-------------------------------------------------------------"
    echo "$1"
    echo "-------------------------------------------------------------"
    echo
}

install_apt_packages() {
    sudo apt-get install -y --no-install-recommends "$@"
}


log "C++ Development Environment Setup"

cat <<EOF
This script will:

  - Update APT package lists
  - Install CMake ${CMAKE_VERSION}
  - Install build tools and compilers
  - Install testing and debugging dependencies
  - Install LLVM/Clang tooling

System changes:

  - Requires sudo privileges
  - Installs packages via apt
  - Installs CMake under /opt/cmake
EOF


log "Installing prerequisites"

install_apt_packages \
    ca-certificates \
    lsb-release \
    wget \
    gnupg

sudo apt-get update


log "Installing CMake ${CMAKE_VERSION}"

if [[ ! -x "/opt/cmake/bin/cmake" ]] || \
   ! "/opt/cmake/bin/cmake" --version | grep -q "cmake version ${CMAKE_VERSION}"; then

    rm -f "${CMAKE_TARBALL}"

    wget -q --show-progress "${CMAKE_URL}" -O "${CMAKE_TARBALL}"

    rm -rf "/tmp/${CMAKE_DIR}"
    tar -xzf "${CMAKE_TARBALL}" -C /tmp

    sudo rm -rf /opt/cmake
    sudo mv "/tmp/${CMAKE_DIR}" /opt/cmake

    rm -f "${CMAKE_TARBALL}"
fi

sudo ln -sf /opt/cmake/bin/cmake /usr/local/bin/cmake
sudo ln -sf /opt/cmake/bin/ctest /usr/local/bin/ctest
sudo ln -sf /opt/cmake/bin/cpack /usr/local/bin/cpack

echo "CMake version:"
cmake --version


log "Installing build tools and compilers"

install_apt_packages \
    build-essential \
    ninja-build \
    clang \
    g++-14 \
    libstdc++-14-dev


log "Installing test and debugging dependencies"

install_apt_packages \
    valgrind \
    libgtest-dev \
    libgmock-dev \
    gcovr


log "Installing LLVM and Clang tooling"

install_apt_packages \
    llvm \
    clang-format \
    clang-tidy \
    llvm-18-tools


log "Verifying installation"

echo "CMake:"
cmake --version | head -n 1

echo
echo "Clang:"
clang --version | head -n 1

echo
echo "GCC:"
g++-14 --version | head -n 1

echo
echo "Ninja:"
ninja --version

echo
echo "clang-format:"
clang-format --version

echo
echo "LLVM:"
llvm-config --version

echo
echo "gcovr:"
gcovr --version | head -n 1

echo
echo "-----------------------------------------------------------------"
echo "All dependencies installed successfully."
echo "-----------------------------------------------------------------"
echo
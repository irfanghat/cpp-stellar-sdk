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

log "Setting up Development Environment..."

sudo apt-get update

log "Installing prerequisites & build utilities"

install_apt_packages \
    ca-certificates \
    lsb-release \
    wget \
    gnupg \
    git \
    gh \
    pkg-config \
    autoconf \
    automake \
    libtool \
    bison \
    flex \
    pandoc

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

log "Installing build tools and compilers"

install_apt_packages \
    build-essential \
    ninja-build \
    clang \
    clangd \
    g++-14 \
    libstdc++-14-dev

log "Verifying Clang C++20 support"

echo "clang:   $(command -v clang)"
echo "clang++: $(command -v clang++)"

clang --version
clang++ --version

echo "Testing C++20 support..."

cat > /tmp/cxx20-test.cpp <<'EOF'
#include <concepts>
#include <type_traits>

template<typename T>
concept Integral = std::is_integral_v<T>;

template<Integral T>
T add(T a, T b) {
    return a + b;
}

int main() {
    return add(1, 2) == 3 ? 0 : 1;
}
EOF

clang++ -std=c++20 /tmp/cxx20-test.cpp -o /tmp/cxx20-test
/tmp/cxx20-test

rm -f /tmp/cxx20-test.cpp /tmp/cxx20-test

echo "Clang C++20 support: OK"

log "Installing xdrpp and xdrc compiler"

if ! command -v xdrc &> /dev/null; then
    XDRPP_TMP="/tmp/xdrpp-build"

    rm -rf "${XDRPP_TMP}"

    git clone --recursive https://github.com/xdrpp/xdrpp.git "${XDRPP_TMP}"

    pushd "${XDRPP_TMP}"

    ./autogen.sh

    CC=clang \
    CXX=clang++ \
    CXXFLAGS="-std=c++20 -O2 -Wno-error" \
    ./configure --disable-doc

    make -j"$(nproc)"

    sudo make install
    sudo ldconfig

    popd

    rm -rf "${XDRPP_TMP}"
fi

log "Installing dev dependencies"

install_apt_packages \
    libsodium-dev

log "Installing test and debugging dependencies"

install_apt_packages \
    valgrind \
    libgtest-dev \
    libgmock-dev \
    libsodium-dev \
    gcovr

log "Installing LLVM and Clang tooling"

install_apt_packages \
    llvm \
    clang-format \
    clang-tidy \
    llvm-18-tools

log "Verifying installation"

echo "CMake:        $(cmake --version | head -n 1)"
echo "Clang:        $(clang --version | head -n 1)"
echo "Clangd:        $(clangd --version | head -n 1)"
echo "GCC:          $(g++-14 --version | head -n 1)"
echo "Ninja:        $(ninja --version)"
echo "xdrc:         $(xdrc --version 2>&1 || echo 'Installed')"
echo "clang-format: $(clang-format --version)"
echo "LLVM:         $(llvm-config --version)"
echo "gcovr:        $(gcovr --version | head -n 1)"

echo
echo "-----------------------------------------------------------------"
echo "All dependencies installed successfully."
echo "-----------------------------------------------------------------"
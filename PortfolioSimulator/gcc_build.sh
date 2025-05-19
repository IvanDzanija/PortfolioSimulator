#!/bin/bash

# Set GCC version — change if you installed a different version
GCC_VERSION=14

# Compiler paths (assumes Homebrew prefix at /opt/homebrew)
export CC="gcc-${GCC_VERSION}"
export CXX="g++-${GCC_VERSION}"

# Directory for build artifacts
BUILD_DIR="build"

# Clean previous build (optional — comment out if not desired)
# rm -rf "$BUILD_DIR"

# Configure with CMake
cmake -S . -B "$BUILD_DIR"

# Build
cmake --build "$BUILD_DIR" -j$(sysctl -n hw.logicalcpu)

echo "✅ Build finished with GCC ${GCC_VERSION}"

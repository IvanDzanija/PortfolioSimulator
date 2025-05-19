#!/bin/bash

# exit immediately if a command exits with a non-zero status
set -e

# Define colors for output
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo -e "${YELLOW}==== Building PortfolioSimulator with Clang + OpenMP ====${NC}"

# Check if Homebrew is installed
if [ ! -d "/opt/homebrew/bin" ]; then
    echo -e "${RED}Error: Homebrew not found in /opt/homebrew/bin${NC}"
    echo "Please install Homebrew or adjust the script to match your Homebrew location"
    exit 1
fi

# Export Homebrew binary path
export PATH="/opt/homebrew/bin:$PATH"

# Check if LLVM/Clang is installed via Homebrew
if [ ! -d "/opt/homebrew/opt/llvm" ]; then
    echo -e "${YELLOW}LLVM/Clang not found. Installing via Homebrew...${NC}"
    brew install llvm
fi

# Check if libomp is installed
if [ ! -d "/opt/homebrew/opt/libomp" ]; then
    echo -e "${YELLOW}OpenMP not found. Installing via Homebrew...${NC}"
    brew install libomp
fi

# Define build directory
BUILD_DIR="build"

# Clean build directory if requested
if [ "$1" == "clean" ]; then
    echo -e "${YELLOW}Cleaning build directory...${NC}"
    rm -rf $BUILD_DIR
fi

# Create build directory if it doesn't exist
mkdir -p $BUILD_DIR

# Get Homebrew LLVM path
LLVM_PATH=$(brew --prefix llvm)
LIBOMP_PATH=$(brew --prefix libomp)

echo -e "${GREEN}Using Clang from: $LLVM_PATH${NC}"
echo -e "${GREEN}Using OpenMP from: $LIBOMP_PATH${NC}"

# Configure with CMake
echo -e "${YELLOW}Running CMake configuration...${NC}"
cmake -S . -B $BUILD_DIR \
    -DCMAKE_C_COMPILER="$LLVM_PATH/bin/clang" \
    -DCMAKE_CXX_COMPILER="$LLVM_PATH/bin/clang++" \
    -DCMAKE_PREFIX_PATH="$LLVM_PATH;$LIBOMP_PATH" \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# Build the project
echo -e "${YELLOW}Building project...${NC}"
cmake --build $BUILD_DIR -j $(sysctl -n hw.ncpu)

# Report success
if [ $? -eq 0 ]; then
    echo -e "${GREEN}Build completed successfully!${NC}"
    echo -e "${GREEN}Executables are in the '$BUILD_DIR' directory${NC}"

else
    echo -e "${RED}Build failed!${NC}"
    exit 1
fi

echo ""
echo -e "${YELLOW}Available executables:${NC}"
find $BUILD_DIR -type f -perm +111 -depth 1

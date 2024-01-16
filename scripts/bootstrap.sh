#!/bin/bash
set -e

echo "=========================================="
echo "SpatialRender CI Lab - Bootstrap Script"
echo "=========================================="
echo ""

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Check if running on Linux or macOS
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    OS="linux"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    OS="macos"
else
    echo -e "${RED}Error: Unsupported operating system${NC}"
    exit 1
fi

echo -e "${GREEN}Detected OS: $OS${NC}"
echo ""

# Function to check if command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to install dependencies on Linux
install_linux_deps() {
    echo -e "${YELLOW}Installing dependencies on Linux...${NC}"
    
    if command_exists apt-get; then
        sudo apt-get update
        sudo apt-get install -y \
            build-essential \
            cmake \
            ninja-build \
            libgl1-mesa-dev \
            libglew-dev \
            libxrandr-dev \
            libxinerama-dev \
            libxcursor-dev \
            libxi-dev \
            xorg-dev \
            ccache \
            python3 \
            python3-pip \
            pkg-config \
            git
    elif command_exists yum; then
        sudo yum install -y \
            gcc-c++ \
            cmake \
            ninja-build \
            mesa-libGL-devel \
            glew-devel \
            ccache \
            python3 \
            python3-pip \
            git
    else
        echo -e "${RED}Error: Unsupported package manager${NC}"
        exit 1
    fi
}

# Function to install dependencies on macOS
install_macos_deps() {
    echo -e "${YELLOW}Installing dependencies on macOS...${NC}"
    
    if ! command_exists brew; then
        echo -e "${RED}Error: Homebrew not found. Please install Homebrew first.${NC}"
        echo "Visit: https://brew.sh"
        exit 1
    fi
    
    brew install cmake ninja glew ccache python3 git
}

# Check and install system dependencies
echo "Checking system dependencies..."
MISSING_DEPS=()

if ! command_exists cmake; then
    MISSING_DEPS+=("cmake")
fi
if ! command_exists ninja; then
    MISSING_DEPS+=("ninja")
fi
if ! command_exists python3; then
    MISSING_DEPS+=("python3")
fi

if [ ${#MISSING_DEPS[@]} -ne 0 ]; then
    echo -e "${YELLOW}Missing dependencies: ${MISSING_DEPS[*]}${NC}"
    read -p "Install missing dependencies? (y/n) " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        if [ "$OS" == "linux" ]; then
            install_linux_deps
        else
            install_macos_deps
        fi
    else
        echo -e "${RED}Please install missing dependencies manually${NC}"
        exit 1
    fi
else
    echo -e "${GREEN}All system dependencies found${NC}"
fi

# Install Python dependencies
echo ""
echo "Installing Python dependencies..."
pip3 install --user numpy pillow || {
    echo -e "${YELLOW}Warning: Failed to install some Python packages. Continuing...${NC}"
}

# Make shader compiler executable
chmod +x tools/shader_compiler.py
chmod +x tests/visual/visual_regression.py

# Compile shaders
echo ""
echo "Compiling shaders..."
python3 tools/shader_compiler.py --all --variant release || {
    echo -e "${YELLOW}Warning: Shader compilation failed. This is OK if shader compilers are not installed.${NC}"
}

# Create build directory
echo ""
echo "Setting up build directory..."
if [ -d "build" ] && [ -f "build/CMakeCache.txt" ]; then
    echo -e "${YELLOW}Cleaning existing build directory...${NC}"
    rm -rf build/CMakeCache.txt build/CMakeFiles
fi
mkdir -p build
cd build

# Configure CMake
echo ""
echo "Configuring CMake..."

# Determine generator (prefer Ninja if available, otherwise use default)
GENERATOR=""
if command_exists ninja; then
    GENERATOR="-G Ninja"
    echo "Using Ninja generator"
else
    echo "Ninja not found, using default generator"
fi

CMAKE_ARGS="-DCMAKE_BUILD_TYPE=Release"
if command_exists ccache; then
    CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_CXX_COMPILER_LAUNCHER=ccache"
fi

if [ -n "$GENERATOR" ]; then
    cmake .. $CMAKE_ARGS $GENERATOR
else
    cmake .. $CMAKE_ARGS
fi

# Build
echo ""
echo "Building project..."
cmake --build . -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

echo ""
echo -e "${GREEN}=========================================="
echo "Bootstrap complete!"
echo "==========================================${NC}"
echo ""
echo "Next steps:"
echo "  1. Run tests: cd build && ctest"
echo "  2. Run visual tests: cd build && ./tests/visual/spatialrender_visual_tests"
echo "  3. Run benchmarks: cd build && ./benchmarks/spatialrender_benchmark"
echo "  4. Run the renderer: cd build && ./spatialrender"
echo ""

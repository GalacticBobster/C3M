#!/bin/bash

# Beer-Lambert 1D Photochemistry Solver - Build and Run Script
# This script demonstrates how to build and run the solver

set -e  # Exit on error

echo "=================================================="
echo "Beer-Lambert 1D Photochemistry Solver"
echo "=================================================="
echo ""

# Check if we're in the right directory
if [ ! -f "BeerLambertSolver1D.cpp" ]; then
    echo "Error: BeerLambertSolver1D.cpp not found!"
    echo "Please run this script from the examples/BeerLambert1D directory"
    exit 1
fi

# Option to clean build
if [ "$1" == "clean" ]; then
    echo "Cleaning build directory..."
    rm -rf build
    echo "Clean complete."
    exit 0
fi

# Create build directory
echo "Step 1: Creating build directory..."
mkdir -p build
cd build

echo ""
echo "Step 2: Configuring with CMake..."
echo "Note: This assumes you have Cantera, Eigen3, and C3M dependencies installed"
echo ""

# Check if C3M_INCLUDE_DIR is set
if [ -z "$C3M_INCLUDE_DIR" ]; then
    # Try to auto-detect
    if [ -d "../../.." ]; then
        export C3M_INCLUDE_DIR=$(cd ../../.. && pwd)
        echo "Auto-detected C3M_INCLUDE_DIR: $C3M_INCLUDE_DIR"
    else
        echo "Warning: C3M_INCLUDE_DIR not set. Using parent directory."
        export C3M_INCLUDE_DIR=$(cd ../.. && pwd)
    fi
fi

# Run CMake
cmake .. || {
    echo ""
    echo "CMake configuration failed!"
    echo ""
    echo "Make sure you have installed:"
    echo "  - Cantera (>= 3.0)"
    echo "  - Eigen3"
    echo "  - C3M source code"
    echo ""
    echo "You may need to set these environment variables:"
    echo "  export C3M_INCLUDE_DIR=/path/to/C3M"
    echo "  export CANTERA_INCLUDE_DIR=/path/to/cantera/include"
    echo "  export CANTERA_LIBRARIES=/path/to/cantera/lib/libcantera.so"
    echo ""
    exit 1
}

echo ""
echo "Step 3: Building the solver..."
make || {
    echo "Build failed!"
    exit 1
}

echo ""
echo "Step 4: Build successful!"
echo ""
echo "The executable is located at: $(pwd)/BeerLambertSolver1D"
echo ""
echo "To run the solver:"
echo "  cd $(pwd)"
echo "  ./BeerLambertSolver1D"
echo ""
echo "The solver will read configuration from beer_lambert_1d.yaml"
echo "and output results to beer_lambert_1d_output.dat"
echo ""
echo "=================================================="
echo "Build Complete!"
echo "=================================================="

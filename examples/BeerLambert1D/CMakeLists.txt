# =============================================
# Beer-Lambert 1D Photochemistry Solver Example
# =============================================

# This example demonstrates a one-dimensional photochemistry solver
# using the Beer-Lambert law for radiative transfer

# Check if this is being built as part of C3M or standalone
if(COMMAND setup_problem)
    # Building as part of C3M - use the setup_problem macro
    
    # 1. Compile the Beer-Lambert solver
    setup_problem(BeerLambertSolver1D)
    
    # 2. Copy input YAML files to run directory
    file(GLOB inputs *.yaml)
    foreach(input ${inputs})
        file(COPY ${input} DESTINATION ${CMAKE_BINARY_DIR}/bin)
    endforeach()
    
else()
    # Building standalone
    cmake_minimum_required(VERSION 3.18)
    project(BeerLambertSolver1D LANGUAGES CXX)
    
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    
    # Find required packages
    find_package(Eigen3 REQUIRED)
    find_package(Cantera REQUIRED)
    
    # Add executable
    add_executable(BeerLambertSolver1D BeerLambertSolver1D.cpp)
    
    # Include directories
    target_include_directories(BeerLambertSolver1D PRIVATE
        ${C3M_INCLUDE_DIR}
        ${EIGEN3_INCLUDE_DIR}
        ${CANTERA_INCLUDE_DIR}
    )
    
    # Link libraries
    target_link_libraries(BeerLambertSolver1D PRIVATE
        ${CANTERA_LIBRARIES}
        ${EIGEN3_LIBRARIES}
    )
    
    # Copy YAML files to build directory
    file(GLOB inputs *.yaml)
    foreach(input ${inputs})
        file(COPY ${input} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    endforeach()
endif()

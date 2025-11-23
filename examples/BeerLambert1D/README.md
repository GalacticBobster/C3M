# Beer-Lambert 1D Photochemistry Solver

## Overview

This example implements a simplified one-dimensional photochemistry solver using the Beer-Lambert law for radiative transfer. The solver computes the attenuation of actinic flux through the atmosphere and calculates photolysis rates at different atmospheric levels.

## Beer-Lambert Law

The Beer-Lambert law describes the attenuation of radiation passing through an absorbing medium:

```
I(z) = I₀ * exp(-τ)
```

where:
- `I(z)` is the intensity at altitude z
- `I₀` is the top-of-atmosphere intensity
- `τ` is the optical depth from TOA to altitude z

The optical depth is calculated as:

```
τ = ∫ σ(λ) * n(z) * dz
```

where:
- `σ(λ)` is the absorption cross-section at wavelength λ
- `n(z)` is the number density of the absorbing species at altitude z

## Files

- **BeerLambertSolver1D.cpp** - Main solver implementation
- **beer_lambert_1d.yaml** - Configuration file for the solver
- **CMakeLists.txt** - Build configuration
- **README.md** - This file

## Building

### Option 1: Build with C3M (if TASK variable is set)

To build this example as part of the C3M build system, you would need to add this to the main examples/CMakeLists.txt:

```cmake
if (${TASK} STREQUAL "BeerLambert")
  add_subdirectory(BeerLambert1D)
endif()
```

Then build with:
```bash
cd C3M
mkdir build
cd build
cmake -DTASK=BeerLambert ..
make
```

### Option 2: Standalone Build

If you want to build this example standalone without modifying existing files:

```bash
cd examples/BeerLambert1D
mkdir build
cd build

# Configure with CMake, pointing to C3M source
cmake -DC3M_INCLUDE_DIR=/path/to/C3M \
      -DCANTERA_INCLUDE_DIR=/path/to/cantera/include \
      -DCANTERA_LIBRARIES=/path/to/cantera/lib/libcantera.so \
      ..

make
```

## Running

After building, run the solver:

```bash
cd build/bin
./BeerLambertSolver1D.release
```

The solver will:
1. Read the configuration from `beer_lambert_1d.yaml`
2. Load the chemical network (photolysis_o2.yaml)
3. Set up a 20-level atmospheric grid from 0-100 km
4. Load stellar radiation data
5. Compute actinic flux using Beer-Lambert law
6. Output results to `beer_lambert_1d_output.dat`

## Configuration

The `beer_lambert_1d.yaml` file contains:

- **network**: Chemical reaction network file
- **atmosphere**: Atmospheric structure (levels, altitude range, pressure, temperature)
- **initial_conditions**: Initial mole fractions for each species
- **output**: Output file configuration

## Dependencies

This solver requires:
- Cantera (>= 3.0)
- Eigen3
- C3M libraries (PhotoChemistry, RadTran, actinic_flux)
- Application framework

## Theory

The solver implements:

1. **Atmospheric Grid Setup**: Creates a vertical grid with exponential pressure profile
2. **Radiation Field**: Converts stellar irradiance to photon flux (actinic flux)
3. **Beer-Lambert Attenuation**: Computes actinic flux at each level by integrating optical depth from top of atmosphere
4. **Photolysis Rates**: Calculates J-values for photodissociation reactions

## Example Output

The solver outputs:
- Altitude [km]
- Pressure [Pa]
- Temperature [K]
- Average actinic flux [photons/s/m²]

at each atmospheric level.

## Notes

- This is a simplified demonstration solver
- For production use, integrate with the full C3M photochemistry framework
- Cross-sections are read from the Cantera reaction network
- The solver can be extended to include time-dependent chemistry evolution

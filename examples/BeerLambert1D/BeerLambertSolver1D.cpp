// @sec3{Beer-Lambert 1D Photochemistry Solver}
// This code implements a simplified one-dimensional photochemistry model
// using the Beer-Lambert law for radiative transfer.
// Author: C3M Development Team
// The solver computes photolysis rates at different atmospheric levels
// using Beer-Lambert attenuation of actinic flux.

// Cantera Solution class - describes a phase with chemical species
#include <cantera/base/Solution.h>

// ThermoPhase object stores the thermodynamic state
#include <cantera/thermo.h>

// Kinetics object stores the chemical kinetics information
#include <cantera/kinetics.h>
#include <cantera/kinetics/Reaction.h>

// Numerical tools
#include <cantera/numerics/eigen_dense.h>
#include <cantera/numerics/eigen_sparse.h>

// Standard library
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>

// C3M headers
#include <c3m/PhotoChemistry.hpp>
#include <c3m/RadTran.hpp>
#include <c3m/actinic_flux.hpp>

// Application
#include <application/application.hpp>

// YAML file IO
#include "cantera/base/ct_defs.h"
#include "cantera/ext/yaml-cpp/yaml.h"

using Eigen::MatrixXd;
using Eigen::VectorXd;
using namespace Cantera;
using namespace std;

// Beer-Lambert law implementation for 1D photochemistry
class BeerLambertSolver1D {
private:
    int nlevels_;          // Number of vertical levels
    int nspecies_;         // Number of chemical species
    int nreactions_;       // Number of reactions
    int nwavelengths_;     // Number of wavelength bins
    
    VectorXd altitude_;    // Altitude grid [m]
    VectorXd pressure_;    // Pressure at each level [Pa]
    VectorXd temperature_; // Temperature at each level [K]
    
    MatrixXd concentration_; // Species concentration at each level [molecules/m^3]
    MatrixXd actinic_flux_;  // Actinic flux at each level and wavelength [photons/s/m^2/m]
    
    VectorXd wavelength_;    // Wavelength grid [m]
    VectorXd toa_flux_;      // Top-of-atmosphere flux [photons/s/m^2/m]
    
    std::shared_ptr<ThermoPhase> gas_;
    std::shared_ptr<Kinetics> kin_;

public:
    BeerLambertSolver1D(int nlevels, std::shared_ptr<ThermoPhase> gas, 
                        std::shared_ptr<Kinetics> kin)
        : nlevels_(nlevels), gas_(gas), kin_(kin) {
        nspecies_ = gas->nSpecies();
        nreactions_ = kin->nReactions();
        
        // Initialize arrays
        altitude_ = VectorXd::Zero(nlevels_);
        pressure_ = VectorXd::Zero(nlevels_);
        temperature_ = VectorXd::Zero(nlevels_);
        concentration_ = MatrixXd::Zero(nlevels_, nspecies_);
    }
    
    // Set up the atmospheric grid
    void setupAtmosphere(const VectorXd& alt, const VectorXd& pres, const VectorXd& temp) {
        altitude_ = alt;
        pressure_ = pres;
        temperature_ = temp;
    }
    
    // Set up the wavelength grid and TOA flux
    void setupRadiation(const std::vector<double>& wavelength, const std::vector<double>& toa_flux) {
        nwavelengths_ = wavelength.size();
        wavelength_ = Eigen::Map<const VectorXd>(wavelength.data(), wavelength.size());
        toa_flux_ = Eigen::Map<const VectorXd>(toa_flux.data(), toa_flux.size());
        actinic_flux_ = MatrixXd::Zero(nlevels_, nwavelengths_);
    }
    
    // Compute actinic flux using Beer-Lambert law
    // I(z) = I_0 * exp(-tau)
    // where tau is the optical depth from TOA to altitude z
    void computeActinicFlux() {
        // Start from the top of atmosphere
        for (int k = 0; k < nwavelengths_; k++) {
            double optical_depth = 0.0;
            
            // Work downward through the atmosphere
            for (int j = nlevels_ - 1; j >= 0; j--) {
                // Set actinic flux using Beer-Lambert attenuation
                actinic_flux_(j, k) = toa_flux_(k) * exp(-optical_depth);
                
                // Calculate layer thickness
                double dz;
                if (j > 0) {
                    dz = altitude_(j) - altitude_(j - 1);
                } else {
                    dz = altitude_(j);
                }
                
                // Calculate number density [molecules/m^3]
                double num_dens = pressure_(j) / (temperature_(j) * Boltzmann);
                
                // Add optical depth contribution from this layer
                // For simplicity, using a representative cross-section
                // In a full implementation, this would loop over all absorbing species
                double total_cross_section = 0.0;
                
                // Sum contributions from all species that absorb at this wavelength
                for (int n = 0; n < nspecies_; n++) {
                    double conc = concentration_(j, n);
                    // Cross section would come from photolysis reaction data
                    // For now, using a placeholder approach
                    double sigma = getCrossSection(n, k, temperature_(j));
                    total_cross_section += conc * sigma;
                }
                
                // Update optical depth
                optical_depth += total_cross_section * dz;
            }
        }
    }
    
    // Get cross section for species at wavelength (placeholder)
    double getCrossSection(int species_idx, int wave_idx, double temp) {
        // In a full implementation, this would read from the reaction data
        // For demonstration, return a small value
        return 1.0e-24; // cm^2 -> m^2 conversion needed
    }
    
    // Set species concentrations
    void setConcentrations(const MatrixXd& conc) {
        concentration_ = conc;
    }
    
    // Compute photolysis rates
    VectorXd computePhotolysisRates(int level, int reaction_idx) {
        VectorXd rates = VectorXd::Zero(nwavelengths_);
        
        for (int k = 0; k < nwavelengths_; k++) {
            // J-value = integral(sigma * phi * F_actinic * d_lambda)
            // where sigma is cross section, phi is quantum yield, F_actinic is actinic flux
            double sigma = getCrossSection(reaction_idx, k, temperature_(level));
            double quantum_yield = 1.0; // Simplified assumption
            rates(k) = sigma * quantum_yield * actinic_flux_(level, k);
        }
        
        return rates;
    }
    
    // Print results
    void printResults() {
        cout << "\n========== Beer-Lambert 1D Photochemistry Solver Results ==========\n";
        cout << "Number of levels: " << nlevels_ << "\n";
        cout << "Number of species: " << nspecies_ << "\n";
        cout << "Number of wavelengths: " << nwavelengths_ << "\n\n";
        
        cout << "Actinic Flux at each level (averaged over wavelengths):\n";
        cout << "Level\tAltitude[km]\tPressure[Pa]\tFlux[photons/s/m^2]\n";
        for (int j = 0; j < nlevels_; j++) {
            double avg_flux = actinic_flux_.row(j).mean();
            cout << j << "\t" << altitude_(j)/1000.0 << "\t\t" 
                 << pressure_(j) << "\t\t" << avg_flux << "\n";
        }
        cout << "\n";
    }
    
    MatrixXd getActinicFlux() const { return actinic_flux_; }
    VectorXd getAltitude() const { return altitude_; }
};

int main(int argc, char** argv) {
    cout << "========== Beer-Lambert 1D Photochemistry Solver ==========\n\n";
    
    // Read input from YAML file
    string fileName = "beer_lambert_1d.yaml";
    cout << "Reading configuration from: " << fileName << "\n";
    
    YAML::Node config = YAML::LoadFile(fileName);
    
    // Load the chemical network
    string network_file = config["network"].as<string>();
    cout << "Loading chemical network from: " << network_file << "\n";
    
    auto sol = newSolution(network_file);
    auto gas = sol->thermo();
    auto gas_kin = sol->kinetics();
    
    int nsp = gas->nSpecies();
    int nrxn = gas_kin->nReactions();
    
    cout << "Number of species: " << nsp << "\n";
    cout << "Number of reactions: " << nrxn << "\n\n";
    
    // Read atmospheric configuration
    int nlevels = config["atmosphere"]["nlevels"].as<int>();
    double z_top = config["atmosphere"]["z_top"].as<double>();
    double z_bottom = config["atmosphere"]["z_bottom"].as<double>();
    double pressure_top = config["atmosphere"]["p_top"].as<double>();
    double pressure_bottom = config["atmosphere"]["p_bottom"].as<double>();
    double temperature = config["atmosphere"]["temperature"].as<double>();
    
    cout << "Setting up atmosphere with " << nlevels << " levels\n";
    cout << "Altitude range: " << z_bottom/1000.0 << " - " << z_top/1000.0 << " km\n";
    cout << "Pressure range: " << pressure_top << " - " << pressure_bottom << " Pa\n";
    cout << "Temperature: " << temperature << " K\n\n";
    
    // Create atmospheric grid (linear in log-pressure)
    VectorXd altitude = VectorXd::LinSpaced(nlevels, z_bottom, z_top);
    VectorXd pressure = VectorXd::Zero(nlevels);
    VectorXd temp = VectorXd::Constant(nlevels, temperature);
    
    // Exponential pressure profile
    double scale_height = 8000.0; // meters
    for (int j = 0; j < nlevels; j++) {
        pressure(j) = pressure_bottom * exp(-(altitude(j) - z_bottom) / scale_height);
    }
    
    // Create solver
    BeerLambertSolver1D solver(nlevels, gas, gas_kin);
    solver.setupAtmosphere(altitude, pressure, temp);
    
    // Setup radiation field
    auto app = Application::GetInstance();
    auto stellar_input_file = app->FindResource("stellar/sun.ir");
    auto stellar_input = ReadStellarRadiationInput(stellar_input_file, 1., 1.);
    
    cout << "Loaded stellar radiation data with " << stellar_input.first.size() << " wavelength bins\n";
    
    // Convert irradiance to actinic flux (photon flux)
    double h = 6.626e-34; // Planck's constant
    double c = 3e8;       // Speed of light
    double factor = 1.0 / (h * c);
    
    std::vector<double> actinic_flux_toa(stellar_input.first.size());
    for (size_t i = 0; i < stellar_input.first.size(); i++) {
        actinic_flux_toa[i] = stellar_input.first[i] * stellar_input.second[i] * factor;
    }
    
    solver.setupRadiation(stellar_input.first, actinic_flux_toa);
    
    // Set initial concentrations
    MatrixXd concentrations = MatrixXd::Zero(nlevels, nsp);
    
    // Set background atmosphere (e.g., N2)
    for (int j = 0; j < nlevels; j++) {
        double num_dens = pressure(j) / (temp(j) * Boltzmann);
        
        // Read initial mole fractions from config
        YAML::Node init_species = config["initial_conditions"]["species"];
        for (auto it = init_species.begin(); it != init_species.end(); ++it) {
            string species_name = it->first.as<string>();
            double mole_frac = it->second.as<double>();
            
            int species_idx = gas->speciesIndex(species_name);
            if (species_idx >= 0) {
                concentrations(j, species_idx) = mole_frac * num_dens;
            }
        }
    }
    
    solver.setConcentrations(concentrations);
    
    // Compute actinic flux using Beer-Lambert law
    cout << "Computing actinic flux using Beer-Lambert law...\n";
    solver.computeActinicFlux();
    
    // Print results
    solver.printResults();
    
    // Write output to file
    string output_file = config["output"]["filename"].as<string>();
    ofstream outfile(output_file);
    
    outfile << "# Beer-Lambert 1D Photochemistry Solver Output\n";
    outfile << "# Altitude[km] Pressure[Pa] Temperature[K] AvgActinicFlux[photons/s/m^2]\n";
    
    MatrixXd flux = solver.getActinicFlux();
    VectorXd alt = solver.getAltitude();
    
    for (int j = 0; j < nlevels; j++) {
        double avg_flux = flux.row(j).mean();
        outfile << alt(j)/1000.0 << " " << pressure(j) << " " 
                << temp(j) << " " << avg_flux << "\n";
    }
    
    outfile.close();
    cout << "\nResults written to: " << output_file << "\n";
    cout << "\nSimulation completed successfully!\n";
    
    return 0;
}

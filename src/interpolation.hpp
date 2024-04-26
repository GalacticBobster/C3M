// C/C++ headers
#include <stdlib.h>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// C3M headers
#include <configure.hpp>

using namespace std;
using Eigen::MatrixXd;
using Eigen::VectorXd;

Eigen::MatrixXd InterpolateCrossSection(Eigen::MatrixXd reference_wavelength,
                                        Eigen::MatrixXd input_wavelength,
                                        Eigen::MatrixXd input_cross_section);
Eigen::MatrixXd InterpolateQYield(Eigen::MatrixXd reference_wavelength,
                                  Eigen::MatrixXd input_wavelength,
                                  Eigen::MatrixXd qyield);

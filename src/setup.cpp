#include "setup.hpp"

void main_setup() { // main_setup for the lbm simulation
    const float Nx = 100u;
    const float Ny = 100u;
    const float Nz = 1u;
    const float nu = 0.1f;
    LBM lbm(Nx, Ny, Nz, nu);
};

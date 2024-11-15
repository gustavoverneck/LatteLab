#include "setup.hpp"

void main_setup() { // main_setup for the lbm simulation
    const uint Nx = 100u;
    const uint Ny = 100u;
    const uint Nz = 1u;
    const float nu = 0.1f;
    const uint N = Nx*Ny*Nz;
    const uint num_threads = 8u;
    const uint timesteps = 1000u;
    

    LBM lbm(Nx, Ny, Nz, nu, num_threads); // Create the LBM object
    lbm.start(); // Start the LBM simulation

    for (int i = 0; i < N; i++) { // Setups grid with initial conditions
        
    };

    //cout << lbm.rho[0] << endl; 

/*
    // Main Loop
    for (int t = 0; t < timesteps; t++) {
        lbm.run(dt);
    };
*/
};

#include "setup.hpp"

void main_setup() { // main_setup for the lbm simulation
    const float Nx = 100u;
    const float Ny = 100u;
    const float Nz = 1u;
    const float nu = 0.1f;
    const float N = Nx*Ny*Nz;
    const uint num_threads = 8u;
    const uint timesteps = 1000u;
    
    LBM lbm(Nx, Ny, Nz, nu, num_threads); // Create the LBM object
    lbm.start(); // Start the LBM simulation
    
    for (int i = 0; i < N; i++) { // Setups grid with initial conditions
        
    };

    // Main Loop
    for (int t = 0; t < timesteps; t++) {
        lbm.run(dt);
    };
};

#include "setup.hpp"

void main_setup() { // main_setup for the lbm simulation
    const uint Nx = 100u;
    const uint Ny = 100u;
    const uint Nz = 1u;
    const float nu = 0.1f;
    const uint N = Nx*Ny*Nz;
    const uint num_threads = 8u;
    const uint timesteps = 100u;
    

    LBM lbm(Nx, Ny, Nz, nu, num_threads); // Create the LBM object
    lbm.start(); // Start the LBM simulation

    for (int n = 0; n < N; n++) { // Setups grid with initial conditions
        vector<uint> result = indexToPosition(n, Nx, Ny, Nz);
        float x = result[0];
        float y = result[1];
        float z = result[2];
        if (x == 0 || x == Nx-1 || y == 0 || y == Ny-1) {
            lbm.flags[n] = TYPE_S;
        } else if (x == 2) {
            lbm.rho[n] = 1.0f;
        } else {
            lbm.rho[n] = 0.1f;
        }
    };

    lbm.run(timesteps); // Run the LBM simulation
    lbm.export_data(); // Export data to a file

};

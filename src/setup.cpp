#include "setup.hpp"

/*
//Basic setup for the LBM simulation
void main_setup() { // main_setup for the lbm simulation
    const uint Nx = 100u;
    const uint Ny = 100u;
    const uint Nz = 1u;
    const float nu = 0.1f;
    const ulong N = Nx*Ny*Nz;
    const uint timesteps = 1000u;

    const uint num_threads = 10u;
    omp_set_num_threads(num_threads);
    

    LBM lbm(Nx, Ny, Nz, nu);
    
    #pragma omp parallel for
    for (ulong n = 0; n < N; n++) { // Setups grid with initial conditions
        vector<uint> p = indexToPosition(n, Nx, Ny, Nz);
        float x = p[0]; float y = p[1]; float z = p[2];

        if (y == 0 || y == Ny-1) {
            lbm.flags[n] = TYPE_S;
            lbm.rho[n] = 1.0f;
            lbm.u[n][0] = 0.0f;
            lbm.u[n][1] = 0.0f;
        } else if (x == 0 && y > 0 && y < Ny-1) {
            lbm.flags[n] = TYPE_IN;
            lbm.rho[n] = 1.0f;
            lbm.u[n][0] = 0.05f;
            lbm.u[n][1] = 0.0f;
        } else if (x == Nx-1 && y > 0 && y < Ny-1) {
            lbm.flags[n] = TYPE_OUT;
            lbm.rho[n] = 1.0f;
            lbm.u[n][0] = 0.0f;
            lbm.u[n][1] = 0.0f;
        } else {
            lbm.flags[n] = TYPE_F;
            lbm.rho[n] = 1.0f;
            lbm.u[n][0] = 0.0f;
            lbm.u[n][1] = 0.0f;
        }
    };

    lbm.set_export_every(200); // Export data every 100 steps

    //lbm.export_data(); // Export data to a file
    lbm.run(timesteps); // Run the LBM simulation

}; // main_setup
*/





// Lid driven cavity setup
void main_setup() { // main_setup for the lbm simulation
    const uint Nx = 256u;
    const uint Ny = 256u;
    const uint Nz = 1u;
    const float nu = 0.1f;
    const ulong N = Nx*Ny*Nz;
    const uint timesteps = 10000u;

    const uint num_threads = 8u;
    omp_set_num_threads(num_threads);
    

    LBM lbm(Nx, Ny, Nz, nu);
    
    #pragma omp parallel for
    for (ulong n = 0; n < N; n++) { // Setups grid with initial conditions
        vector<uint> p = indexToPosition(n, Nx, Ny, Nz);
        float x = p[0]; float y = p[1]; float z = p[2];

        if (y == 0 || x == 0 || x == Nx-1) {
            lbm.flags[n] = TYPE_S;
            lbm.rho[n] = 1.0f;
            lbm.u[n][0] = 0.0f;
            lbm.u[n][1] = 0.0f;
        } else if (y == Ny-1) {
            lbm.flags[n] = TYPE_IN;
            lbm.rho[n] = 1.0f;
            lbm.u[n][0] = 0.1f;
            lbm.u[n][1] = 0.0f;
        } else {
            lbm.flags[n] = TYPE_F;
            lbm.rho[n] = 1.0f;
            lbm.u[n][0] = 0.0f;
            lbm.u[n][1] = 0.0f;
        }
    };

    lbm.set_export_every(1000); // Export data every 100 steps

    //lbm.export_data(); // Export data to a file
    lbm.run(timesteps); // Run the LBM simulation

}; // main_setup

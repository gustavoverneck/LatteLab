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
    omp_set_default_device(1); // Ensure GPU is used

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
            lbm.flags[n] = TYPE_S;
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

    lbm.set_export_every(100); // Export data every 100 steps

    //lbm.export_data(); // Export data to a file
    lbm.run(timesteps); // Run the LBM simulation

}; // main_setup
/**/



/*
// Lid driven cavity setup
void main_setup() { // main_setup for the lbm simulation
    const uint Nx = 128u;
    const uint Ny = 128u;
    const uint Nz = 1u;
    const float nu = 0.1f;
    const ulong N = Nx*Ny*Nz;
    const uint timesteps = 500;

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

    lbm.set_export_every(100); // Export data every 100 steps
    lbm.run(timesteps); // Run the LBM simulation
    //lbm.export_data(); // Export data to a file

}; // main_setup
/**/




// 2D Taylor-Green Vortex setup
void main_setup() { // main_setup for the lbm simulation
    const uint Nx = 200u;
    const uint Ny = 200u;
    const uint Nz = 1u;
    const float nu = 0.10f;
    const ulong N = Nx*Ny*Nz;
    const uint timesteps = 1000;
    const uint u0 = 0.1f;
    const uint Nv = 4u; // Number of vortices per dimension
    const float Lx = Nx / Nv;
    const float Ly = Ny / Nv;

    LBM lbm(Nx, Ny, Nz, nu);

    #pragma omp parallel for
    for (ulong n = 0; n < N; n++) { // Setups grid with initial conditions
        vector<uint> p = indexToPosition(n, Nx, Ny, Nz);
        float x = p[0]; float y = p[1]; float z = p[2];

        if (x == 0 || x == Nx-1 || y == 0 || y == Ny-1) {
            lbm.flags[n] = TYPE_S;
            lbm.rho[n] = 1.0f;
            lbm.u[n][0] = 0.0f;
            lbm.u[n][1] = 0.0f;
        } else {
            lbm.flags[n] = TYPE_F;
            lbm.rho[n] = 1.0f + (u0 * u0 / (4 * nu)) * (cos(4 * PIF * Nv * x / Lx) + cos(4 * PIF * Nv * y / Ly));
            lbm.u[n][0] = u0 * cos(2 * PIF * Nv * x / Lx) * sin(2 * PIF * Nv * y / Ly);
            lbm.u[n][1] = -u0 * sin(2 * PIF * Nv * x / Lx) * cos(2 * PIF * Nv * y / Ly);
        }
    };
    lbm.set_export_every(100); // Export data every 100 steps
    lbm.run(timesteps); // Run the LBM simulation

}; // main_setup
/**/
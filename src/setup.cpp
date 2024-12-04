#include "setup.hpp"

// LBM simulation setups

/*
// Lid driven cavity setup
void main_setup() { // main_setup for the lbm simulation
    const uint Nx = 100u;
    const uint Ny = 100u;
    const uint Nz = 1u;
    const float u0 = 0.1f;
    const float Re = 100.0f;
    const float nu = nu_from_reynolds(Re, u0, Nx);
    const ulong N = Nx*Ny*Nz;
    const uint timesteps = 1000;

    LBM lbm(Nx, Ny, Nz, nu);

    #pragma omp parallel for
    for (ulong n = 0; n < N; n++) { // Setups grid with initial conditions
        vector<uint> p = indexToPosition(n, Nx, Ny, Nz);
        float x = p[0]; float y = p[1]; float z = p[2];

        if (y == 0 || ((x == 0 || x == Nx-1) && y < Ny-1)) {
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

}; // main_setup
/**/


/*
// 2D Taylor-Green Vortex setup
void main_setup() { // main_setup for the lbm simulation
    const uint Nx = 128u;
    const uint Ny = 128u;
    const uint Nz = 1u;
    const float nu = 0.0314f;
    const ulong N = Nx*Ny*Nz;
    const uint timesteps = 1000;
    const float u0 = 0.1f;
    const float Nv = 1.0f; // Number of vortices per dimension
    const float Lx = Nx / Nv;
    const float Ly = Ny / Nv;

    LBM lbm(Nx, Ny, Nz, nu);

    #pragma omp parallel for
    for (ulong n = 0; n < N; n++) { // Setups grid with initial conditions
        vector<uint> p = indexToPosition(n, Nx, Ny, Nz);
        uint x = p[0]; uint y = p[1]; uint z = p[2];

        // Taylor-Green vortex initial conditions
        lbm.flags[n] = TYPE_IN;
        lbm.u[n][0] = -u0 * cos(2.0f * M_PI * x / Nx) * sin(2.0f * M_PI * y / Ny);
        lbm.u[n][1] = u0 * sin(2.0f * M_PI * x / Nx) * cos(2.0f * M_PI * y / Ny);
        lbm.rho[n] = 1.0f;
    }
    lbm.set_export_every(50); // Export data every 100 steps
    lbm.run(timesteps); // Run the LBM simulation
} // main_setup
/**/




//  2D Von-Karman vortex setup
void main_setup() { // main_setup for the lbm simulation
    const uint R = 16u; // Radius of the sphere
    const uint Nx = 32u*R; // Number of cells in the x direction
    const uint Ny = 16u*R; // Number of cells in the y direction
    const uint Nz = 1u;
    const float Re = 250.0f;
    const float u0 = 0.10f;
    const float nu = nu_from_reynolds(Re, u0, Ny);
    const ulong N = Nx*Ny*Nz;
    const uint timesteps = 1000;

    LBM lbm(Nx, Ny, Nz, nu);

    #pragma omp parallel for
    for (ulong n = 0; n < N; n++) { // Setups grid with initial conditions
        vector<uint> p = indexToPosition(n, Nx, Ny, Nz);
        uint x = p[0]; uint y = p[1]; uint z = p[2];
        float r = sqrt((x - 50) * (x - 50) + (y - Ny / 2) * (y - Ny / 2));
        if (x == 1) {
            lbm.flags[n] = TYPE_IN;
            lbm.rho[n] = 1.0f;
            lbm.u[n][0] = u0;
            lbm.u[n][1] = 0.0f;
        } else if (x == 0) {
            lbm.flags[n] = TYPE_OUT;
            lbm.rho[n] = 1.0f;
            lbm.u[n][0] = 0.0f;
            lbm.u[n][1] = 0.0f;
        } else if (r <= R || x == 0) {
            lbm.flags[n] = TYPE_S; // Solid sphere
            lbm.rho[n] = 1.0f;
            lbm.u[n][0] = 0.0f;
            lbm.u[n][1] = 0.0f;
        } else {
            lbm.flags[n] = TYPE_F;
            lbm.rho[n] = 0.0f;
            lbm.u[n][0] = 0.0f;
            lbm.u[n][1] = 0.0f;
        }
    }
    
    lbm.set_export_every(50); // Export data every 100 steps
    lbm.run(timesteps); // Run the LBM simulation
} // main_setup
/**/




/*
// 2D Couette flow setup
void main_setup() { // main_setup for the lbm simulation
    const uint Nx = 128u;
    const uint Ny = 128u;
    const uint Nz = 1u;
    const float nu = 0.006f;
    const ulong N = Nx * Ny * Nz;
    const uint timesteps = 10000;
    const float u0 = 0.1f; // Velocity of the top plateot

    LBM lbm(Nx, Ny, Nz, nu);

    #pragma omp parallel for        
    for (ulong n = 0; n < N; n++) { // Setups grid with initial conditions
        vector<uint> p = indexToPosition(n, Nx, Ny, Nz);
        uint x = p[0]; uint y = p[1]; uint z = p[2];

        if (y == 0 || y == Ny - 1) {
            lbm.flags[n] = TYPE_S;
            lbm.rho[n] = 1.0f;
            lbm.u[n][0] = 0.0f;
            lbm.u[n][1] = 0.0f;
        } else if (y == Ny - 2) {
            lbm.flags[n] = TYPE_IN;
            lbm.rho[n] = 1.0f;
            lbm.u[n][0] = u0;
            lbm.u[n][1] = 0.0f;
        } else {
            lbm.flags[n] = TYPE_F;
            lbm.rho[n] = 1.0f;
            lbm.u[n][0] = 0.0f;
            lbm.u[n][1] = 0.0f;
        }

    }

    lbm.set_export_every(100); // Export data every 100 steps
    lbm.run(timesteps); // Run the LBM simulation
}
/**/

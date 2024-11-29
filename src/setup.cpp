#include "setup.hpp"

//Basic setup for the LBM simulation
/*
void main_setup() { // main_setup for the lbm simulation
    const uint Nx = 128u;
    const uint Ny = 128u;
    const uint Nz = 1u;
    const float Re = 100.0f;
    const float nu = nu_from_reynolds(Re, 0.1f, Nx);
    const ulong N = Nx*Ny*Nz;
    const uint timesteps = 1000;
    omp_set_num_threads(8);
    LBM lbm(Nx, Ny, Nz, nu);

    // Period boundary conditions (will be applied at every time step)
    lbm.apply([&](LBM& lbm, uint n, uint x, uint y, uint z) {
        if (x == Nx-1) {
            lbm.u[n][0] = 0.1f;
        };
    });

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



/*
// Lid driven cavity setup
void main_setup() { // main_setup for the lbm simulation
    const uint Nx = 128u;
    const uint Ny = 128u;
    const uint Nz = 1u;
    const float Re = 100.0f;
    const float nu = nu_from_reynolds(Re, 0.1f, Nx);
    const ulong N = Nx*Ny*Nz;
    const uint timesteps = 1000;
    omp_set_num_threads(8);
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
    const float Re = 100.0f;
    const float nu = nu_from_reynolds(Re, 0.1f, Nx);
    const ulong N = Nx*Ny*Nz;
    const uint timesteps = 1000;
    const uint u0 = 0.1f;
    const uint Nv = 1u; // Number of vortices per dimension
    const float Lx = Nx / Nv;
    const float Ly = Ny / Nv;

    LBM lbm(Nx, Ny, Nz, nu);

    // Period boundary conditions (will be applied at every time step)
    lbm.apply([&](LBM& lbm, uint n, uint x, uint y, uint z) {
        lbm.flags[n] = TYPE_F;
    });


    #pragma omp parallel for
    for (ulong n = 0; n < N; n++) { // Setups grid with initial conditions
        vector<uint> p = indexToPosition(n, Nx, Ny, Nz);
        uint x = p[0]; uint y = p[1]; uint z = p[2];

        lbm.flags[n] = TYPE_IN;
        lbm.rho[n] = 1.0f + (u0 * u0 / (4 * nu)) * (cos(4 * PIF * Nv * x / Lx) + cos(4 * PIF * Nv * y / Ly));
        lbm.u[n][0] = u0 * cos(2 * PIF * Nv * x / Lx) * sin(2 * PIF * Nv * y / Ly);
        lbm.u[n][1] = u0 * sin(2 * PIF * Nv * x / Lx) * cos(2 * PIF * Nv * y / Ly);
    }
    lbm.set_export_every(100); // Export data every 100 steps
    lbm.run(timesteps); // Run the LBM simulation
} // main_setup
/**/




/*
//  Periodic Boundary test setup
void main_setup() { // main_setup for the lbm simulation
    const uint Nx = 200u;
    const uint Ny = 100u;
    const uint Nz = 1u;
    const float Re = 120.0f;
    const float nu = nu_from_reynolds(Re, 0.1f, Nx);
    const ulong N = Nx*Ny*Nz;
    const uint timesteps = 3000;
    const uint u0 = 0.1f;
    const uint Nv = 1u; // Number of vortices per dimension
    const float Lx = Nx / Nv;
    const float Ly = Ny / Nv;

    LBM lbm(Nx, Ny, Nz, nu);

    #pragma omp parallel for
    for (ulong n = 0; n < N; n++) { // Setups grid with initial conditions
        vector<uint> p = indexToPosition(n, Nx, Ny, Nz);
        uint x = p[0]; uint y = p[1]; uint z = p[2];
        if (x == Nx/2 && y > Ny/3 && y < 2*Ny/3) {
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
    }
    lbm.set_export_every(100); // Export data every 100 steps
    lbm.run(timesteps); // Run the LBM simulation
} // main_setup
/**/



//  2D Von-Karman vortex setup
void main_setup() { // main_setup for the lbm simulation
    const uint Nx = 400u; // Number of cells in the x direction
    const uint Ny = 200u; // Number of cells in the y direction
    const uint Nz = 1u;
    const float Re = 2000.0f;
    const float u0 = 0.01f;
    const float nu = nu_from_reynolds(Re, u0, Ny);
    const ulong N = Nx*Ny*Nz;
    const uint timesteps = 10000;

    LBM lbm(Nx, Ny, Nz, nu, true);

    #pragma omp parallel for
    for (ulong n = 0; n < N; n++) { // Setups grid with initial conditions
        vector<uint> p = indexToPosition(n, Nx, Ny, Nz);
        uint x = p[0]; uint y = p[1]; uint z = p[2];
        float r = sqrt((x - 75) * (x - 75) + (y - Ny / 2) * (y - Ny / 2));
        if (x == 1) {
            lbm.flags[n] = TYPE_IN;
            lbm.rho[n] = 1.0f;
            lbm.u[n][0] = u0;
            lbm.u[n][1] = 0.0f;
        } else if (x == 0) {
            lbm.flags[n] = TYPE_OUT;
            lbm.rho[n] = 1.0f;
            lbm.u[n][0] = u0;
            lbm.u[n][1] = 0.0f;
        } else if (r <= 18u || x == 0) {
            lbm.flags[n] = TYPE_S; // Solid sphere
            lbm.rho[n] = 1.0f;
            lbm.u[n][0] = 0.0f;
            lbm.u[n][1] = 0.0f;
        } else {
            lbm.flags[n] = TYPE_F;
            lbm.rho[n] = 1.0f;
            lbm.u[n][0] = u0;
            lbm.u[n][1] = 0.0f;
        }
    }
    
    lbm.set_export_every(100); // Export data every 100 steps
    lbm.run(timesteps); // Run the LBM simulation
} // main_setup
/**/
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
/**/





// Lid driven cavity setup
void main_setup() { // main_setup for the lbm simulation
    const uint Nx = 128;
    const uint Ny = 128u;
    const uint Nz = 1u;
    const float nu = 0.1f;
    const ulong N = Nx*Ny*Nz;
    const uint timesteps = 100u;

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



/*
// 2D Taylor-Green Vortex setup
void main_setup() { // main_setup for the lbm simulation
    const uint Nx = 1024u;
    const uint Ny = 1024u;
    const uint Nz = 1u;
    const float nu = 0.02f;
    const ulong N = Nx*Ny*Nz;
    const uint timesteps = 2000;
    const uint u0 = 0.1f;
    const uint Nv = 2u; // Number of vortices per dimension

    const uint num_threads = 8u;
    omp_set_num_threads(num_threads);
    omp_set_default_device(1); // Ensure GPU is used

    LBM lbm(Nx, Ny, Nz, nu);
    
    #pragma omp parallel for
    for (ulong n = 0; n < N; n++) { // Setups grid with initial conditions
        vector<uint> p = indexToPosition(n, Nx, Ny, Nz);
        float x = p[0]; float y = p[1]; float z = p[2];

        if (y == 0 || x == 0 || x == Nx-1 || y == Ny-1) {
            lbm.flags[n] = TYPE_S;
            lbm.rho[n] = 1.0f;
            lbm.u[n][0] = 0.0f;
            lbm.u[n][1] = 0.0f;
        } else {
            lbm.flags[n] = TYPE_F;
            const float a=(float)Nx/(float)Nv, b=(float)Ny/(float)Nv;
            const float fx = (float)x+0.5f-0.5f*(float)Nx;
            const float fy = (float)y+0.5f-0.5f*(float)Ny;
            lbm.u[n][0] = u0 * cosf(2.0f * pif * fx / a) * sinf(2.0f * pif * fy / b);
            lbm.u[n][1] = -u0 * sinf(2.0f * pif * fx / a)*cosf(2.0f * pif * fy / b);;
            lbm.rho[n] =  1.0f - sqrt(u0) * 3.0f / 4.0f * (cosf(4.0f * pif * fx / a)+cosf(4.0f * pif * fy / b));
        }
    };

    lbm.set_export_every(100); // Export data every 100 steps

    //lbm.export_data(); // Export data to a file
    lbm.run(timesteps); // Run the LBM simulation

}; // main_setup
/**/
// src/core/kernels.rs

pub const LBM_KERNEL: &str = r#"
// Velocity vectors (D2Q9, D3Q7, D3Q15, D3Q19, D3Q27)
constant int c[Q][3] = {
#if defined(D2Q9)
    {0, 0, 0}, {1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0},
    {1, 1, 0}, {-1, -1, 0}, {1, -1, 0}, {-1, 1, 0}
#elif defined(D3Q7)
    {0, 0, 0}, {1, 0, 0}, {-1, 0, 0}, {0, 1, 0},
    {0, -1, 0}, {0, 0, 1}, {0, 0, -1}
#elif defined(D3Q15)
    {0, 0, 0}, {1, 0, 0}, {-1, 0, 0}, {0, 1, 0},
    {0, -1, 0}, {0, 0, 1}, {0, 0, -1}, {1, 1, 1},
    {-1, -1, -1}, {1, 1, -1}, {-1, -1, 1}, {1, -1, 1},
    {-1, 1, -1}, {-1, 1, 1}, {1, -1, -1}
#elif defined(D3Q19)
    {0, 0, 0}, {1, 0, 0}, {-1, 0, 0}, {0, 1, 0},
    {0, -1, 0}, {0, 0, 1}, {0, 0, -1}, {1, 1, 0},
    {-1, -1, 0}, {1, 0, 1}, {-1, 0, -1}, {0, 1, 1},
    {0, -1, -1}, {1, -1, 0}, {-1, 1, 0}, {1, 0, -1},
    {-1, 0, 1}, {0, 1, -1}, {0, -1, 1}
#elif defined(D3Q27)
    {0, 0, 0}, {1, 0, 0}, {-1, 0, 0}, {0, 1, 0},
    {0, -1, 0}, {0, 0, 1}, {0, 0, -1}, {1, 1, 0},
    {-1, -1, 0}, {1, 0, 1}, {-1, 0, -1}, {0, 1, 1},
    {0, -1, -1}, {1, -1, 0}, {-1, 1, 0}, {1, 0, -1},
    {-1, 0, 1}, {0, 1, -1}, {0, -1, 1}, {1, 1, 1},
    {-1, -1, -1}, {1, 1, -1}, {-1, -1, 1}, {1, -1, 1},
    {-1, 1, -1}, {-1, 1, 1}, {1, -1, -1}
#endif
};

// Weights for the velocity sets
constant float w[Q] = {
#if defined(D2Q9)
    4.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0,
    1.0 / 36.0, 1.0 / 36.0, 1.0 / 36.0, 1.0 / 36.0
#elif defined(D3Q7)
    1.0 / 4.0, 1.0 / 8.0, 1.0 / 8.0, 1.0 / 8.0, 1.0 / 8.0, 1.0 / 8.0, 1.0 / 8.0
#elif defined(D3Q15)
    2.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0,
    1.0 / 9.0, 1.0 / 9.0, 1.0 / 72.0, 1.0 / 72.0, 1.0 / 72.0,
    1.0 / 72.0, 1.0 / 72.0, 1.0 / 72.0, 1.0 / 72.0, 1.0 / 72.0
#elif defined(D3Q19)
    1.0 / 3.0, 1.0 / 18.0, 1.0 / 18.0, 1.0 / 18.0, 1.0 / 18.0,
    1.0 / 18.0, 1.0 / 18.0, 1.0 / 36.0, 1.0 / 36.0, 1.0 / 36.0,
    1.0 / 36.0, 1.0 / 36.0, 1.0 / 36.0, 1.0 / 36.0, 1.0 / 36.0,
    1.0 / 36.0, 1.0 / 36.0, 1.0 / 36.0, 1.0 / 36.0
#elif defined(D3Q27)
    8.0 / 27.0, 2.0 / 27.0, 2.0 / 27.0, 2.0 / 27.0, 2.0 / 27.0,
    2.0 / 27.0, 2.0 / 27.0, 1.0 / 54.0, 1.0 / 54.0, 1.0 / 54.0,
    1.0 / 54.0, 1.0 / 54.0, 1.0 / 54.0, 1.0 / 54.0, 1.0 / 54.0,
    1.0 / 54.0, 1.0 / 54.0, 1.0 / 54.0, 1.0 / 54.0, 1.0 / 216.0,
    1.0 / 216.0, 1.0 / 216.0, 1.0 / 216.0, 1.0 / 216.0, 1.0 / 216.0,
    1.0 / 216.0, 1.0 / 216.0
#endif
};

// Streaming kernel
__kernel void streaming_kernel(
    __global float* f, __global float* f_new) {
    // Get global IDs (node coordinates in the 1D grid)
    int n = get_global_id(0);

    // Check if the thread is within the grid boundaries
    if (n >= N) return;

    // Calculate the coordinates (x, y, z) from the linear index n
    int x = n % NX;
    int y = (n / NX) % NY;
    int z = n / (NX * NY);

    // Loop over the directions
    for (int q = 0; q < Q; q++) {
        // Calculate the coordinates of the neighboring node
        int x_nbr = x + c[q][0];
        int y_nbr = y + c[q][1];
        int z_nbr = z + c[q][2];

        // Apply periodic boundary conditions
        x_nbr = (x_nbr + NX) % NX;  // Wrap around x within grid bounds
        y_nbr = (y_nbr + NY) % NY;  // Wrap around y within grid bounds
        z_nbr = (z_nbr + NZ) % NZ;  // Wrap around z within grid bounds

        // Calculate the linear index of the neighboring node
        int n_nbr = x_nbr + y_nbr * NX + z_nbr * NX * NY;

        // Perform the streaming of the distribution function
        f_new[n_nbr * Q + q] = f[n * Q + q];
    }

}

// Collision kernel (BGK model)
__kernel void collision_kernel(__global float* f, __global float* rho, __global float* u, float omega) {
    // Get global IDs (node coordinates in the 1D grid)
    int n = get_global_id(0);

    // Check if the thread is within the grid boundaries
    if (n >= N) return;

    // Calculate density and velocity (momentum)
    float local_rho = 0.0f;
    float ux = 0.0f;
    float uy = 0.0f;
    float uz = 0.0f;


    for (int q = 0; q < Q; q++) {
        local_rho += f[n * Q + q];
        ux += c[q][0] * f[n * Q + q];
        uy += c[q][1] * f[n * Q + q];
        uz += c[q][2] * f[n * Q + q];
    }

    // Normalize velocity
    if (local_rho >= 1.0e-10) {
        ux /= local_rho;
        uy /= local_rho;
        uz /= local_rho;
    } else {
        ux = 0.0;
        uy = 0.0;
        uz = 0.0;
    }

    // Calculate the equilibrium distribution function and apply the BGK model
    for (int q = 0; q < Q; q++) {
        float cu = c[q][0]*ux + c[q][1]*uy + c[q][2]*uz;

        float u2 = ux * ux + uy * uy + uz * uz;
        float feq = local_rho * w[q] * (1.0f + 3.0f * cu + 4.5f * cu * cu - 1.5f * u2);
        f[n*Q + q] = f[n*Q + q] * (1.0f - omega) + feq * omega;
    }

    
    
    // Store the computed density and velocity
    rho[n] = local_rho;
    u[n * 3] = ux;
    u[n * 3 + 1] = uy;
    u[n * 3 + 2] = uz;
}
"#;
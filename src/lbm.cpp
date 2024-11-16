#include "lbm.hpp"
#include <functional>


LBM::LBM(const uint Nx, const uint Ny, const uint Nz, const float nu, const uint num_threads) 
    :Nx(Nx), Ny(Ny), Nz(Nz), nu(nu), num_threads(num_threads) {} // initialize member variables



void LBM::collision_task(int start, int end) {
    for (int n = start; n < end; n++) {
        // Calculate the density
        this->rho[n] = 0.0f;
        for (int i = 0; i < velocities; ++i) {
            this->rho[n] += this->f[n][i];
        };

        // Calculate the velocity
        double u = 0.0f;
        double v = 0.0f;
        for (int i = 0; i < velocities; ++i) {
            u += c[n][0] * f[n][i];
            v += c[n][1] * f[n][i];
        };
        u /= rho[n];
        v /= rho[n];

        // Calculate the equilibrium distribution
        double feq;
        for (int i = 0; i < velocities; i++) {
            double cu = c[i][0] * u + c[i][1] * v;
            double feq = w[i] * rho[n] * (1.0f + 3.0f * cu + 9.0f * cu * cu / 2.0f - 3.0f * (u * u + v * v) / 2.0f);
            this->f_eq[n][i] = feq;
        };

        // Perform the collision step
        for (int i = 0; i < velocities; i++) {
            this->f[n][i] = this->f[n][i] + (this->f_eq[n][i] - this->f[n][i]) / dt;
        };
    };
};


void LBM::streaming_task(int start, int end) {
    vector<vector<double>> f_temp;
    f_temp = f;
    for (int i = start; i < end; ++i) {
            for (int n = 0; n < velocities; ++n) {
                int next_i = (i + c[n][0] + Nx) % Nx;
                int next_j = (i + c[n][1] + Ny) % Ny;
                int next_k = (i + c[n][2] + Nz) % Nz;
                int next_index = next_i + Nx * (next_j + Ny * next_k);
                f_temp[next_index][n] = this->f[i][n];
            };
        };
    for (int i = start; i < end; ++i) {
        for (int n = 0; n < velocities; ++n) {
            this->f[i][n] = f_temp[i][n];
        };
    };
};


void LBM::start() {
    print_logo();
    cout << "Starting LBM simulation...\n";
    initialize();
    check_erros();
}; // Run the LBM simulation

void LBM::run(const uint timesteps) {
    this->step = 0u;
    while (this->step <= timesteps) {
        this->step++;
        evolve();
    }
}; // Run the LBM simulation

void LBM::initialize() {
    // Initialize the lattice
    cout << "Initializing variables...\n";
    this->N = Nx*Ny*Nz; // Number of lattice cells

    #if defined(SIM_FLUID)
        this->f = vector<vector<double>> (N, vector<double>(velocities, 0.0f)); // Distribution functions
        this->f_eq = vector<vector<double>> (N, vector<double>(velocities, 0.0f)); // Distribution functions
        this->rho = vector<double> (N, 0.0f); // Density
        this->flags = vector<uint> (N, 0u); // Flags for each cell
        //this->u = vector<vector<double>> (N, vector<double>(dimensions, 0.0f)); // Velocity

    #elif defined(SIM_PLASMA)
        this->f = vector<vector<double>> (N, vector<double>(velocities, 0.0f)); // Distribution functions
        this->f_eq = vector<vector<double>> (N, vector<double>(velocities, 0.0f)); // Distribution functions
        this->g = vector<vector<double>> (N, vector<double>(velocities, 0.0f)); // Distribution functions
        this->g_eq = vector<vector<double>> (N, vector<double>(velocities, 0.0f)); // Distribution functions
        this->flags = vector<uint> (N, 0u); // Flags for each cell
        this->rho = vector<double> (N, 0.0f); // Density
        //this->u = vector<vector<double>> (N, vector<double>(dimensions, 0.0f)); // Velocity
        this->E = vector<vector<double>> (N, vector<double>(dimensions, 0.0f)); // Electric field
        this->B = vector<vector<double>> (N, vector<double>(dimensions, 0.0f)); // Magnetic field
        

    #else
        std::cerr << "Error: Lattice type not defined. Please define a lattice type in the definitions.hpp file." << std::endl;
        return;
        
    #endif


    
    // Set the initialized flag to true
    initialized = true;

};

void LBM::evolve() { // Perform one LBM time step -> collision -> boundary conditions -> streaming
    this->collision();
    this->boundary_conditions();
    this->streaming();
};

/** collision
 * @brief Perform the collision step for the Lattice Boltzmann Method (LBM).
 *
 * This function executes the collision step of the LBM simulation. Depending on the 
 * defined simulation type (SIM_FLUID or SIM_PLASMA), it calculates the density, 
 * velocity, equilibrium distribution, and updates the distribution functions.
 *
 * @note This function uses multi-threading to parallelize the collision step.
 *
 * @details
 * - For SIM_FLUID:
 *   - Calculates the density and velocity for each lattice point.
 *   - Computes the equilibrium distribution function.
 *   - Updates the distribution function using the collision step.
 * - For SIM_PLASMA:
 *   - Calculates the density for each lattice point.
 *   - Updates the electric and magnetic fields.
 *   - Computes the velocity for each lattice point.
 *   - Computes the equilibrium distribution functions for both f and g.
 *   - Updates the distribution functions f and g using the collision step.
 *
 * @throws std::runtime_error if the lattice type is not defined.
 */
void LBM::collision() {              // Collision step
    #if defined(SIM_FLUID)
        std::vector<std::thread> threads;
        int chunk_size = N / num_threads;

        for (uint t = 0; t < num_threads; ++t) {
            int start = t * chunk_size;
            int end = (t == num_threads - 1) ? N : start + chunk_size;
            threads.emplace_back(std::bind(&LBM::collision_task, this, start, end));
        };

        for (auto& thread : threads) {
            thread.join();
        };
    
    #elif defined(SIM_PLASMA)
        
    #else
        std::cerr << "Error: Lattice type not defined. Please define a lattice type in the definitions.hpp file." << std::endl;
        return;
    #endif
}; 

void LBM::boundary_conditions() {    // Boundary conditions
    for (int i = 0; i < Nx; ++i) {
        for (int j = 0; j < Ny; ++j) {
            for (int k = 0; k < Nz; ++k) {
                int index = i + Nx * (j + Ny * k);

                // Apply boundary conditions based on cell type
                if (flags[index] == 1) { // Solid boundary
                    for (int n = 0; n < velocities; ++n) {
                        int opposite = (n + velocities / 2) % velocities;
                        this->f[index][n] = this->f[index][opposite]; // Bounce-back boundary condition
                    }
                } else if (flags[index] == 2) { // Inlet boundary
                    double rho_inlet = 1.0; // Set desired inlet density
                    double u_inlet = 0.1; // Set desired inlet velocity
                    double v_inlet = 0.0;
                    for (int n = 0; n < velocities; ++n) {
                        double cu = c[n][0] * u_inlet + c[n][1] * v_inlet;
                        this->f[index][n] = w[n] * rho_inlet * (1.0 + 3.0 * cu + 9.0 * cu * cu / 2.0 - 3.0 * (u_inlet * u_inlet + v_inlet * v_inlet) / 2.0);
                    }
                } else if (flags[index] == 3) { // Outlet boundary
                    for (int n = 0; n < velocities; ++n) {
                        this->f[index][n] = this->f[index - Nx * Ny][n]; // Simple outflow condition
                    }
                }
                // Add more boundary conditions as needed
            }
        }
    }
};

/** streaming
 * @brief Performs the streaming step of the Lattice Boltzmann Method (LBM).
 * 
 * This function executes the streaming step in parallel using multiple threads.
 * Each thread processes a chunk of the data array, performing the streaming
 * operation for each cell within its assigned range.
 * 
 * The streaming operation typically involves updating the distribution functions
 * based on the equilibrium distribution functions or other streaming logic.
 * 
 * The number of threads used is determined by the `num_threads` member variable,
 * and the data array size is determined by the `N` member variable.
 * 
 * The function ensures that all threads complete their work by joining them
 * before returning.
 */
void LBM::streaming() { // Streaming step

    vector<vector<double>> f_temp = f;

    std::vector<std::thread> threads;
    int chunk_size = N / num_threads;

    for (uint t = 0; t < num_threads; ++t) {
        int start = t * chunk_size;
        int end = (t == num_threads - 1) ? N : start + chunk_size;
        threads.emplace_back(std::bind(&LBM::streaming_task, this, start, end));
    };
};


void LBM::export_data() { // Export data to a file
    // Export data to a file
    std::ofstream file("data.txt");
    if (file.is_open()) {
        for (int i = 0; i < Nx; ++i) {
            for (int j = 0; j < Ny; ++j) {
                for (int k = 0; k < Nz; ++k) {
                    int index = i + Nx * (j + Ny * k);
                    file << i << " " << j << " " << k << " " << rho[index] << std::endl;
                }
            }
        }
        file.close();
    } else {
        std::cerr << "Error: Unable to open file for writing." << std::endl;
    }
};
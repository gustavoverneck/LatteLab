#include "lbm.hpp"
#include <functional>


LBM::LBM(const uint Nx, const uint Ny, const uint Nz, const float nu, const uint num_threads) 
    :Nx(Nx), Ny(Ny), Nz(Nz), nu(nu), num_threads(num_threads) {

        // Initialize the lattice
        cout << "Initializing variables...\n";
        this->N = Nx*Ny*Nz; // Number of lattice cells

        #if defined(SIM_FLUID)
            this->f = vector<vector<double>> (N, vector<double>(velocities, 0.0f)); // Distribution functions
            this->f_eq = vector<vector<double>> (N, vector<double>(velocities, 0.0f)); // Distribution functions
            this->f_temp = vector<vector<double>> (N, vector<double>(velocities, 0.0f)); // Distribution functions
            this->rho = vector<double> (N, 0.0f); // Density
            this->flags = vector<uint> (N, 0u); // Flags for each cell
            this->u = vector<vector<double>> (N, vector<double>(dimensions, 0.0f)); // Velocity

            // Setups grid with initial conditions
            for (int n = 0; n < N; n++) {
                // Calculate the velocity

                
                // Calculate the equilibrium distribution
                for (int i = 0; i < velocities; i++) {
                    double uu = this->u[n][0] * this->u[n][0] + this->u[n][1] * this->u[n][1];
                    double cu = c[i][0] * this->u[n][0] + c[i][1] * this->u[n][1];
                    this->f_eq[n][i] = w[i] * this->rho[n] * (1.0f + (3.0f * cu)/cs2 + (9.0f * cu * cu)/(2.0f*cs*cs)  - (3.0f * uu)/(2.0f*cs2));
                }

                // Perform the collision step
                for (int i = 0; i < velocities; i++) {
                    this->f[n][i] = this->f[n][i] + (this->f_eq[n][i] - this->f[n][i]) * dt;
                }
        }

        #elif defined(SIM_PLASMA)
            this->f = vector<vector<double>> (N, vector<double>(velocities, 0.0f)); // Distribution functions
            this->f_eq = vector<vector<double>> (N, vector<double>(velocities, 0.0f)); // Distribution functions
            this->f_temp = vector<vector<double>> (N, vector<double>(velocities, 0.0f)); // Distribution functions
            this->g = vector<vector<double>> (N, vector<double>(velocities, 0.0f)); // Distribution functions
            this->g_eq = vector<vector<double>> (N, vector<double>(velocities, 0.0f)); // Distribution functions
            this->g_temp = vector<vector<double>> (N, vector<double>(velocities, 0.0f)); // Distribution functions
            this->rho = vector<double> (N, 0.0f); // Density
            this->flags = vector<uint> (N, 0u); // Flags for each cell
            this->u = vector<vector<double>> (N, vector<double>(dimensions, 0.0f)); // Velocity
            //this->u = vector<vector<double>> (N, vector<double>(dimensions, 0.0f)); // Velocity
            this->E = vector<vector<double>> (N, vector<double>(dimensions, 0.0f)); // Electric field
            this->B = vector<vector<double>> (N, vector<double>(dimensions, 0.0f)); // Magnetic field

        #else
            std::cerr << "Error: Lattice type not defined. Please define a lattice type in the definitions.hpp file." << std::endl;
            return;
            
        #endif

        // Set the initialized flag to true
        initialized = true;
}

 // ---------------------------------------------------------------------------------------------------------

/** check_erros
 * @brief Checks for errors and warnings in the LBM object.
 *
 * This function verifies if the LBM object has been properly initialized
 * and checks for specific conditions based on the defined macros.
 * 
 * @note If the LBM object is not initialized, an error message will be printed
 *       to the standard error output.
 * 
 * @warning For D2Q9 configurations, this function ensures that Nz is set to 1u.
 *          If Nz is not 1u, an error message will be printed to the standard error output.
 */
void LBM::check_erros() { // Check for errors and warnings

            if (!initialized) {
                std::cerr << "LBM object not initialized. Call LBM::initialize() before calling this function." << std::endl;
                return;
            };
        #ifdef D2Q9
            if(Nz!=1u) std::cerr << "D2Q9 can not have Nz=1u! Change it in the constructor." << std::endl;
        #endif // D2Q9
}  // check_erros()


// ---------------------------------------------------------------------------------------------------------

void LBM::collision_task(int start, int end) {
    
}

// ---------------------------------------------------------------------------------------------------------

void LBM::streaming_task(int start, int end) {
    
}

// ---------------------------------------------------------------------------------------------------------


void LBM::start() {
    print_logo();
    cout << "Starting LBM simulation...\n";
    this->check_erros();
} // Run the LBM simulation

// ---------------------------------------------------------------------------------------------------------


void LBM::run(const uint timesteps) {
    this->step = 0u;
    cout << "Running LBM simulation for " << timesteps << " timesteps...\n";
    while (this->step <= timesteps && timesteps > 0) {
        this->step++;
        this->evolve();
    }
} // Run the LBM simulation

// ---------------------------------------------------------------------------------------------------------


void LBM::evolve() { // Perform one LBM time step -> collision -> boundary conditions -> streaming
    this->collision();
    this->boundary_conditions();
    this->streaming();
}

// ---------------------------------------------------------------------------------------------------------


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
        int chunk_size = this->N / this->num_threads;

        for (uint t = 0; t < this->num_threads; ++t) {
            int start = t * chunk_size;
            int end = (t == this->num_threads - 1) ? this->N : start + chunk_size;
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
} 

// ---------------------------------------------------------------------------------------------------------

/** boundary_conditions
 * @brief Applies boundary conditions to the Lattice Boltzmann Method (LBM) simulation.
 *
 * This function iterates through all cells in the simulation domain and applies the appropriate
 * boundary conditions based on the cell type. The boundary conditions include:
 * 
 * - Solid boundary (flags[index] == 1): Applies the bounce-back boundary condition where the
 *   distribution function is reflected back.
 * - Inlet boundary (flags[index] == 2): Sets the distribution function based on a desired inlet
 *   density and velocity using the equilibrium distribution function.
 * - Outlet boundary (flags[index] == 3): Applies a simple outflow condition where the distribution
 *   function is copied from the adjacent cell in the inflow direction.
 *
 * Additional boundary conditions can be added as needed.
 */
void LBM::boundary_conditions() {    // Boundary conditions
    
}

// ---------------------------------------------------------------------------------------------------------

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

    std::vector<std::thread> threads;
    int chunk_size = this->N / num_threads;

    for (uint t = 0; t < num_threads; ++t) {
        int start = t * chunk_size;
        int end = (t == num_threads - 1) ? this->N : start + chunk_size;
        threads.emplace_back(std::bind(&LBM::streaming_task, this, start, end));
    };
    
    for (auto& thread : threads) {
            thread.join();
        }; 
    
    for (int n = 0; n < N; ++n) {
        for (int i = 0; i < velocities; ++i) {
            this->f[n][i] = this->f_temp[n][i];
        }
    }
}

// ---------------------------------------------------------------------------------------------------------

/** export_data
 * @brief Exports the simulation data to a file.
 *
 * This function writes the simulation data to a file named "data.txt".
 * The data includes the indices (i, j, k) and the corresponding density (rho) values.
 * The file is structured such that each line contains the indices and the density value
 * separated by spaces.
 *
 * @note The file is overwritten if it already exists.
 * @note If the file cannot be opened for writing, an error message is printed to the standard error.
 */
void LBM::export_data() { // Export data to a file
    // Export data to a file
    std::ofstream file("data.txt");
    if (file.is_open()) {
        file << "i" << "\t" << "j" << "\t" << "k" << "\t" << "rho" << "\t" << "|u|" << std::endl;
        for (int i = 0; i < Nx; ++i) {
            for (int j = 0; j < Ny; ++j) {
                for (int k = 0; k < Nz; ++k) {
                    int index = i + Nx * (j + Ny * k);
                    double uu = sqrt(this->u[index][0] * this->u[index][0] + this->u[index][1] * this->u[index][1]);
                    file << i << "\t" << j << "\t" << k << "\t" << this->rho[index] << "\t" << uu << "\n";
                };
            };
        };
        file.close();
    } else {
        std::cerr << "Error: Unable to open file for writing." << std::endl;    }
}
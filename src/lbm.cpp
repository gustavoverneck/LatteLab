#include "lbm.hpp"
#include <functional>

/** LBM
 * @brief Constructs an LBM object with the given dimensions and viscosity.
 * 
 * @param Nx The number of cells in the x-dimension.
 * @param Ny The number of cells in the y-dimension.
 * @param Nz The number of cells in the z-dimension.
 * @param nu The kinematic viscosity of the fluid.
 */
LBM::LBM(const uint Nx, const uint Ny, const uint Nz, const float nu) 
    :Nx(Nx), Ny(Ny), Nz(Nz), nu(nu) {
        this->start();
} // LBM


/** init
 * @brief Initializes the Lattice Boltzmann Method (LBM) simulation.
 * 
 * This function sets up the lattice structure, distribution functions, density,
 * velocity, and other necessary variables for the LBM simulation based on the 
 * defined lattice type (D2Q9, D3Q15, D3Q19, D3Q27) and simulation type (SIM_FLUID, SIM_PLASMA).
 * 
 * @note The lattice type and simulation type must be defined in the definitions.hpp file.
 * 
 * @details
 * - Initializes the lattice cells count (N) based on the dimensions (Nx, Ny, Nz).
 * - Sets the lattice velocity vectors (c) according to the defined lattice type.
 * - Initializes distribution functions (f, f_eq, f_temp) and other variables (rho, flags, u)
 *   for fluid simulations.
 * - Initializes additional distribution functions (g, g_eq, g_temp) and fields (E, B)
 *   for plasma simulations.
 * - Sets the initialized flag to true upon successful initialization.
 * 
 * @throws std::runtime_error if the lattice type is not defined.
 */
void LBM::init() {
// Initialize the lattice
    
    // Set the number of threads to 8 if the number of threads is greater than or equal to 8
    omp_set_num_threads(THREADS);
    cout << "Threads: " << omp_get_max_threads() << endl;


    this->N = Nx*Ny*Nz; // Number of lattice cells
    this->tau = nu * 3.0f / dt + 0.5f; // Relaxation time

    #ifdef D2Q9
        this->c = vector<vector<int>> {{0,0},{1,0},{0,1},{-1,0},{0,-1},{1,1},{-1,1},{-1,-1},{1,-1}};
    #elif defined(D3Q15)
        this->c = vector<vector<int>> {{0,0,0},{1,0,0},{0,1,0},{-1,0,0},{0,-1,0},{0,0,1},{0,0,-1},{1,1,0},{-1,1,0},{-1,-1,0},{1,-1,0},{1,0,1},{-1,0,1},{-1,0,-1},{1,0,-1}};
    #elif defined(D3Q19)
        this->c = vector<vector<int>> {{0,0,0},{1,0,0},{0,1,0},{-1,0,0},{0,-1,0},{0,0,1},{0,0,-1},{1,1,0},{-1,1,0},{-1,-1,0},{1,-1,0},{1,0,1},{-1,0,1},{-1,0,-1},{1,0,-1},{0,1,1},{0,-1,1},{0,-1,-1},{0,1,-1}};
    #elif defined(D3Q27)
        this->c = vector<vector<int>> {{0,0,0},{1,0,0},{0,1,0},{-1,0,0},{0,-1,0},{0,0,1},{0,0,-1},{1,1,0},{-1,1,0},{-1,-1,0},{1,-1,0},{1,0,1},{-1,0,1},{-1,0,-1},{1,0,-1},{0,1,1},{0,-1,1},{0,-1,-1},{0,1,-1},{1,1,1},{-1,1,1},{-1,-1,1},{1,-1,1},{1,1,-1},{-1,1,-1},{-1,-1,-1},{1,-1,-1}};
    #endif

    #if defined(SIM_FLUID)
        this->f = vector<vector<double>> (N, vector<double>(velocities, 0.0f)); // Distribution functions
        //this->f_eq = vector<vector<double>> (N, vector<double>(velocities, 0.0f)); // Distribution functions # UPDATE: f_eq is no longer a vector of size N. It will be computed every timestep for memmory optimization
        this->f_temp = vector<vector<double>> (N, vector<double>(velocities, 0.0f)); // Distribution functions
        this->rho = vector<double> (N, 1.0f); // Density
        this->flags = vector<uint> (N, 0u); // Flags for each cell
        this->u = vector<vector<double>> (N, vector<double>(dimensions, 0.0f)); // Velocity

    #elif defined(SIM_PLASMA)
        this->f = vector<vector<double>> (N, vector<double>(velocities, 0.0f)); // Distribution functions
        //this->f_eq = vector<vector<double>> (N, vector<double>(velocities, 0.0f)); // Distribution functions # UPDATE: f_eq is no longer a vector of size N. It will be computed every timestep for memmory optimization
        this->f_temp = vector<vector<double>> (N, vector<double>(velocities, 0.0f)); // Distribution functions
        this->g = vector<vector<double>> (N, vector<double>(velocities, 0.0f)); // Distribution functions
        this->g_eq = vector<vector<double>> (N, vector<double>(velocities, 0.0f)); // Distribution functions
        this->g_temp = vector<vector<double>> (N, vector<double>(velocities, 0.0f)); // Distribution functions
        this->rho = vector<double> (N, 0.0f); // Density
        this->flags = vector<uint> (N, 0u); // Flags for each cell
        this->u = vector<vector<double>> (N, vector<double>(dimensions, 0.0f)); // Velocity
        this->E = vector<vector<double>> (N, vector<double>(dimensions, 0.0f)); // Electric field
        this->B = vector<vector<double>> (N, vector<double>(dimensions, 0.0f)); // Magnetic field

    #else
        std::cerr << "Error: Lattice type not defined. Please define a lattice type in the definitions.hpp file." << std::endl;
        return;
        
    #endif

    // Start the lattice with the equilibrium distribution function
    #pragma omp parallel for
    for (uint n = 0; n < this->N; n++) {
        if (this->flags[n] == TYPE_S) continue; // Skip solid cells
        vector<double> f_eq = this->compute_feq(n);
        for (uint i = 0; i < velocities; i++) {
            this->f[n][i] = f_eq[i];
        }
    }

    // Set the initialized flag to true
    this->initialized = true;
} // init


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

    if (!this->initialized) {
        std::cerr << "LBM object not initialized. Call LBM::initialize() before calling this function." << std::endl;
        return;
    };

    #ifdef D2Q9
        if(Nz!=1u) std::cerr << "D2Q9 can not have Nz=1u! Change it in the constructor." << std::endl;
    #endif // D2Q9

    // Nu warning
    if (nu < 0.0f) {
        std::cerr << "Error: Kinematic viscosity (nu) is negative." << std::endl;
    } else if (nu > 0.5) {
        cout << "Warning: Kinematic viscosity (nu) is greater than 0.5, which can cause instabilities" << std::endl;
    } else if (nu >= 1.0f) {
        cout << "Warning: Kinematic viscosity (nu) should not be much greater than to 1.0." << std::endl;
    }
    

}  // check_erros()


// ---------------------------------------------------------------------------------------------------------


/** start
 * @brief Starts the LBM (Lattice Boltzmann Method) simulation.
 * 
 * This function initializes the LBM simulation by performing the following steps:
 * 1. Prints the logo.
 * 2. Outputs a message indicating the start of the simulation.
 * 3. Calls the init() function to initialize the simulation parameters.
 * 4. Calls the check_erros() function to verify if there are any errors before running the simulation.
 */
void LBM::start() {
    print_logo();
    cout << "Starting LBM simulation...\n";
    this->init();
    this->check_erros();
} // start


// ---------------------------------------------------------------------------------------------------------


/**
 * @brief Runs the LBM (Lattice Boltzmann Method) simulation for a specified number of timesteps.
 *
 * This function initializes the simulation step to zero and then iteratively evolves the simulation
 * state until the specified number of timesteps is reached. It prints a message indicating the start
 * of the simulation and the total number of timesteps to be executed.
 *
 * @param timesteps The number of timesteps to run the simulation for.
 */
void LBM::run(const uint timesteps) {
    this->timesteps = timesteps;
    this->step = 0u;
    cout << "Running LBM simulation for " << timesteps << " timesteps..." << endl;
    while (this->step <= timesteps && timesteps > 0) {
        this->step++;
        this->evolve();
        this->print_progress();
    };
    cout << endl;
} // run


// ---------------------------------------------------------------------------------------------------------


/** evolve
 * @brief Perform one Lattice Boltzmann Method (LBM) time step.
 *
 * This function executes a single time step of the LBM simulation. The process
 * involves three main stages:
 * 1. Collision: Handles the collision step where particle distribution functions
 *    are updated based on local interactions.
 * 2. Boundary Conditions: Applies the necessary boundary conditions to the
 *    simulation domain.
 * 3. Streaming: Updates the particle distribution functions by streaming them
 *    to neighboring nodes.
 */
void LBM::evolve() { // Perform one LBM time step -> collision -> boundary conditions -> streaming
    this->collision();
    this->boundary_conditions();
    this->streaming();
    
    if (bool_export_every) this->export_data(); // Export data every export_every steps

} // evolve


// ---------------------------------------------------------------------------------------------------------


/** compute_feq
 * @brief Computes the equilibrium distribution function (f_eq) for a given node.
 *
 * This function calculates the equilibrium distribution function for a given node
 * in the lattice Boltzmann method (LBM) simulation. The computation is based on
 * the fluid velocity and density at the node.
 *
 * @param n The index of the node for which the equilibrium distribution function is computed.
  */
vector<double> LBM::compute_feq(const uint n) {
    vector<double> f_eq(velocities, 0.0f);
    for (int i = 0; i < velocities; i++) {
        double uc = (this->u[n][0] * this->c[i][0] + this->u[n][1] * this->c[i][1]);
        f_eq[i] = w[i] * this->rho[n] *(1 + 3.0f * uc + 4.5f * uc * uc - 1.5f * (this->u[n][0] * this->u[n][0] + this->u[n][1] * this->u[n][1]));
    };
    return f_eq;
} // compute_feq

// ---------------------------------------------------------------------------------------------------------

#if defined(SIM_PLASMA)
    void LBM::compute_geq(const uint n) {

    } // compute_geq
#endif


// ---------------------------------------------------------------------------------------------------------


/** collision
 * @brief Perform the collision step of the Lattice Boltzmann Method (LBM).
 *
 * This function executes the collision step, which is a crucial part of the LBM simulation.
 * It computes the density and velocity for each cell and updates the distribution functions.
 * The collision step is parallelized using OpenMP to improve performance.
 *
 * @note This function handles different simulation types based on preprocessor directives.
 *       - SIM_FLUID: Fluid simulation
 *       - SIM_PLASMA: Plasma simulation (logic not implemented)
 *       - If neither is defined, an error message is printed.
 *
 * @warning Ensure that the lattice type is defined before calling this function.
 */
void LBM::collision() {              // Collision step
    #if defined(SIM_FLUID)
        // Parallelize the collision process using OpenMP
        #pragma omp parallel for
        for (uint n = 0; n < this->N; n++) {
            if (this->flags[n] == TYPE_S || this->flags[n] == TYPE_IN || this->flags[n] == TYPE_OUT) continue; // Skip solid and in/out cells
            // Compute density and velocity for each cell
            this->rho[n] = 0.0f;
            this->u[n][0] = 0.0f;
            this->u[n][1] = 0.0f;
            for (int i = 0; i < velocities; i++) {
                this->rho[n] += this->f[n][i];
                this->u[n][0] += this->f[n][i] * this->c[i][0];
                this->u[n][1] += this->f[n][i] * this->c[i][1];
            }
            this->u[n][0] /= this->rho[n];
            this->u[n][1] /= this->rho[n];

            for (int i = 0; i < velocities; i++) {
                if (i != 0u) {
                    vector<double> f_eq = this->compute_feq(n);
                    this->f[n][i] = (1 - dt / this->tau) * f[n][i] + (dt / this->tau) * f_eq[i];
                };
            }
        };

    #elif defined(SIM_PLASMA)
        // Plasma simulation logic goes here
    #else
        std::cerr << "Error: Lattice type not defined." << std::endl;
        return;
    #endif
}  // collision


// ---------------------------------------------------------------------------------------------------------


/** boundary_conditions
 * @brief Applies boundary conditions to the Lattice Boltzmann Method (LBM) simulation.
 *
 * This function iterates through all cells in the simulation domain and applies the appropriate
 * boundary conditions based on the cell type. The boundary conditions include:
 * 
 * - Solid boundary (flags[index] == TYPE_S): Applies the bounce-back boundary condition where the
 *   distribution function is reflected back.
 * - Inlet boundary (flags[index] == TYPE_IN): Sets the distribution function based on a desired inlet
 *   density and velocity using the equilibrium distribution function.
 * - Outlet boundary (flags[index] == TYPE_OUT): Applies a simple outflow condition where the distribution
 *   function is copied from the adjacent cell in the inflow direction.
 *
 * Additional boundary conditions can be added as needed.
 */
void LBM::boundary_conditions() {    // Boundary conditions
    // Apply boundary conditions
    if (this->bc){
        this->apply(bc); // Apply boundary conditions over time steps
    };
    #if defined (SIM_FLUID)
        #if defined (D2Q9)
            f_temp = f; // Copy the distribution functions to a temporary array
            #pragma omp parallel for
            for (uint n = 0u; n < this->N; n++) {
                if (this->flags[n] == TYPE_S) { // Solid Boundary -> bounce-back
                    vector<uint> neighbors_index = getNeighbors(n, this->Nx, this->Ny, this->Nz);
                    for (uint nn : neighbors_index) {
                        if (this->flags[nn] == TYPE_S) continue; // Skip solid cells
                        uint i = getDirectionIndex(n, nn, this->Nx, this->Ny, this->Nz); // Get the direction index
                        if (i == 1u) { 
                            this->f_temp[nn][getOpositeDirection(1)] = this->f[n][1];
                            this->f_temp[nn][getOpositeDirection(5)] = this->f[n][5];
                            this->f_temp[nn][getOpositeDirection(8)] = this->f[n][8];
                        } else if (i == 2) {
                            this->f_temp[nn][getOpositeDirection(2)] = this->f[n][2];
                            this->f_temp[nn][getOpositeDirection(5)] = this->f[n][5];
                            this->f_temp[nn][getOpositeDirection(6)] = this->f[n][6];
                        } else if (i == 3) {
                            this->f_temp[nn][getOpositeDirection(3)] = this->f[n][3];
                            this->f_temp[nn][getOpositeDirection(6)] = this->f[n][5];
                            this->f_temp[nn][getOpositeDirection(7)] = this->f[n][7];
                        } else if (i == 4) {
                            this->f_temp[nn][getOpositeDirection(4)] = this->f[n][4];
                            this->f_temp[nn][getOpositeDirection(7)] = this->f[n][7];
                            this->f_temp[nn][getOpositeDirection(8)] = this->f[n][8];
                        } else {
                            this->f_temp[nn][getOpositeDirection(i)] = this->f[n][i];
                        }
                    }
                } else if (this->flags[n] == TYPE_IN) { // Inlet Boundary
                    // Set the distribution function based on the desired inlet density and velocity
                    vector<double> f_eq = this->compute_feq(n);
                    for (uint i = 0u; i < velocities; i++) {
                        this->f_temp[n][i] = f_eq[i];
                    }
                } else if (this->flags[n] == TYPE_OUT) { // Outlet Boundary
                    // Simple outflow condition
                    uint nx = (n % this->Nx) - 1;
                    uint ny = (n / this->Nx);
                    if (nx >= 0 && nx < this->Nx && ny >= 0 && ny < this->Ny) {
                        uint dest = nx + ny * this->Nx;
                        for (uint i = 0u; i < velocities; i++) {
                            this->f_temp[n][i] = this->f[dest][i];
                        }
                    }
                } else if (this->flags[n] == TYPE_F) continue;
            }
            // Copiar os valores atualizados de volta para `f`
            #pragma omp parallel for
            for (int n = 0; n < this->N; n++) {
                this->f[n] = this->f_temp[n];
            }

        #elif defined (D3Q15) || defined (D3Q19) || defined (D3Q27)

        #endif

    #elif defined(SIM_PLASMA)

    #else
        std::cerr << "Error: Lattice type not defined." << std::endl;
        return;
    #endif
}

// ---------------------------------------------------------------------------------------------------------


void LBM::streaming() { // Streaming step
    #if defined(SIM_FLUID)
        #if defined(D2Q9)
            this->f_temp = this->f; // Copy the distribution functions to a temporary array
            // Streaming step
            #pragma omp parallel for
            for (int n = 0; n < this->N; n++) {
                if (this->flags[n] == TYPE_S || this->flags[n] == TYPE_OUT) {
                    continue;   // Skip solid cells
                } else {
                    vector<uint> neighbors_index = getNeighbors(n, this->Nx, this->Ny, this->Nz);
                    for (uint nn : neighbors_index) {
                        if (this->flags[nn] == (TYPE_S || TYPE_IN || TYPE_OUT)) continue; // Skip solid cells
                        uint i = getDirectionIndex(n, nn, this->Nx, this->Ny, this->Nz); // Get the direction index
                        this->f_temp[nn][i] = this->f[n][i];
                    };
                };
            };

            // Copiar os valores atualizados de volta para `f`
            #pragma omp parallel for
            for (int n = 0; n < this->N; n++) {
                this->f[n] = this->f_temp[n];
            }

        #elif defined(D3Q15)
        #elif defined(D3Q19)
        #elif defined(D3Q27)
        #endif
    
    #elif defined(SIM_PLASMA)
        // Plasma simulation logic goes here
    #else
        std::cerr << "Error: Lattice type not defined." << std::endl;
        return;
    #endif
}

// ---------------------------------------------------------------------------------------------------------

/** getNeighbors
 * @brief Get the direction index based on the provided vector.
 * 
 * This function returns the index of the direction vector `v` in the predefined
 * set of direction vectors `c`. The function works for both 2D and 3D cases,
 * depending on the defined macros (D2Q9, D3Q15, D3Q19, D3Q27).
 * 
 * @param v The direction vector for which the index is to be found.
 * @param Nx The number of grid points in the x-direction (not used in this function).
 * @param Ny The number of grid points in the y-direction (not used in this function).
 * @param Nz The number of grid points in the z-direction (not used in this function).
 * @return uint The index of the direction vector `v` in the predefined set of direction vectors `c`.
 */
uint LBM::getDirectionIndex(uint n, uint nn, uint Nx, uint Ny, uint Nz) {
    #if defined (D2Q9)
        vector<uint> p = indexToPosition(n, Nx, Ny, Nz); // Get the position of current cell
        vector<uint> np = indexToPosition(nn, Nx, Ny, Nz); // Get the position of the neighbor cell

        //if (np[0] == p[0] && np[1] == p[1]) return 0; // Center is not needed

        if (np[0] == (p[0] + 1) % Nx && np[1] == p[1]) return 1; // East
        if (np[0] == p[0] && np[1] == (p[1] + 1) % Ny) return 2; // North
        if (np[0] == (p[0] - 1 + Nx) % Nx && np[1] == p[1]) return 3; // West
        if (np[0] == p[0] && np[1] == (p[1] - 1 + Ny) % Ny) return 4; // South
        if (np[0] == (p[0] + 1) % Nx && np[1] == (p[1] + 1) % Ny) return 5; // Northeast
        if (np[0] == (p[0] - 1 + Nx) % Nx && np[1] == (p[1] + 1) % Ny) return 6; // Northwest
        if (np[0] == (p[0] - 1 + Nx) % Nx && np[1] == (p[1] - 1 + Ny) % Ny) return 7; // Southwest
        if (np[0] == (p[0] + 1) % Nx && np[1] == (p[1] - 1 + Ny) % Ny) return 8; // Southeast
        throw std::runtime_error("Error: Direction not found.");
    #elif defined (D3Q15)
    #elif defined (D3Q19)
    #elif defined (D3Q27)
    #endif
    throw std::runtime_error("Error: Direction not found.");
    return 0;
} // getDirectionIndex

// ---------------------------------------------------------------------------------------------------------
// Function to apply boundary conditions over time steps
void LBM::apply(std::function<void(LBM&, uint, uint, uint, uint)> bc) {
    if (!this->bc) {
        this->bc = bc;
    } else {
        #pragma omp parallel for
        for (uint n = 0; n < this->N; n++) {
            vector<uint> p = indexToPosition(n, this->Nx, this->Ny, this->Nz);
            uint x = p[0]; uint y = p[1]; uint z = p[2];
            bc(*this, n, x, y, z);
        }
    }
} // apply




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
    if (bool_export_every && (this->step % this->export_interval == 0 || this->step == 1)) {
        std::ofstream file("exports/data_" + std::to_string(step) + ".csv");
        if (file.is_open()) {
            file << "x" << ",\t" << "y" << ",\t" << "z" << ",\t" << "rho" << ",\t" << "u_x" << ",\t" << "u_y" << "\n";
            for (uint n = 0; n < this->N; n++) {
                vector<uint> p = indexToPosition(n, this->Nx, this->Ny, this->Nz);
                file << p[0] << ",\t" << p[1] << ",\t" << p[2] << ",\t" << this->rho[n] << ",\t" << this->u[n][0] << ",\t" << this->u[n][1] << "\n";
            };
            file.close();
        } else {
            std::cerr << "Error: Unable to open file for writing." << std::endl;
        }
    } else if (!bool_export_every && this->step == this->timesteps-1) {
        std::ofstream file("exports/data.csv");
        if (file.is_open()) {
            file << "x" << ",\t" << "y" << ",\t" << "z" << ",\t" << "rho" << ",\t" << "u_x" << ",\t" << "u_y" << "\n";
            for (uint n = 0; n < this->N; n++) {
                vector<uint> p = indexToPosition(n, this->Nx, this->Ny, this->Nz);
                file << p[0] << ",\t" << p[1] << ",\t" << p[2] << ",\t" << this->rho[n] << ",\t" << this->u[n][0] << ",\t" << this->u[n][1] << "\n";
            };
            file.close();
        } else {
            std::cerr << "Error: Unable to open file for writing." << std::endl;    }
    }
}

void LBM::set_export_every(const uint interval) {
    this->export_interval = interval;
    this->bool_export_every = true;
}



// ---------------------------------------------------------------------------------------------------------
void LBM::print_progress() {
    static auto start_time = std::chrono::high_resolution_clock::now();
    auto current_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = current_time - start_time;
    double estimated_total_time = elapsed.count() * this->timesteps / this->step;
    double estimated_time_left = estimated_total_time - elapsed.count();

    int elapsed_hours = int(elapsed.count()) / 3600;
    int elapsed_minutes = (int(elapsed.count()) % 3600) / 60;
    int elapsed_seconds = int(elapsed.count()) % 60;

    int left_hours = int(estimated_time_left) / 3600;
    int left_minutes = (int(estimated_time_left) % 3600) / 60;
    int left_seconds = int(estimated_time_left) % 60;

    cout << "\r"; // Clear the current line
    cout << "Step: " << this->step << " / " << this->timesteps;
    cout << " | Elapsed time: " << elapsed_hours << "h " << elapsed_minutes << "m " << elapsed_seconds << "s";
    cout << " | ETC: " << left_hours << "h " << left_minutes << "m " << left_seconds << "s";
    cout.flush();
} // print_progress

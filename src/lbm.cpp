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
        cout << "Initializing variables...\n";
        this->N = Nx*Ny*Nz; // Number of lattice cells

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
            this->f_eq = vector<vector<double>> (N, vector<double>(velocities, 0.0f)); // Distribution functions
            this->f_temp = vector<vector<double>> (N, vector<double>(velocities, 0.0f)); // Distribution functions
            this->rho = vector<double> (N, 1.0f); // Density
            this->flags = vector<uint> (N, 0u); // Flags for each cell
            this->u = vector<vector<double>> (N, vector<double>(dimensions, 0.0f)); // Velocity

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
    this->step = 0u;
    cout << "Running LBM simulation for " << timesteps << " timesteps...\n";
    while (this->step <= timesteps && timesteps > 0) {
        this->step++;
        this->evolve();
    }
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
    //this->boundary_conditions();
    this->streaming();
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
void LBM::compute_feq(const uint n) {
    for (int i = 0; i < velocities; i++) {
        double uc = (this->u[n][0] * this->c[i][0] + this->u[n][1] * this->c[i][1]);
        this->f_eq[n][i] = w[i] * this->rho[n] *(1 + 3.0f * uc + 4.5f * uc * uc - 1.5f * (this->u[n][0] * this->u[n][0] + this->u[n][1] * this->u[n][1]));
    };
} // compute_feq

// ---------------------------------------------------------------------------------------------------------

#if defined(SIM_PLASMA)
    void LBM::compute_geq(const uint n) {

    } // compute_geq
#endif

// ---------------------------------------------------------------------------------------------------------





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
        // Parallelize the collision task using OpenMP
        #pragma omp parallel for
        for (uint n = 0; n < this->N; n++) {
            for (int i = 0; i < velocities; i++) {
                this->f[n][i] = this->rho[n] * this->f[n][i];
            }
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
            if (this->rho[n] > 1e-10) {
                this->u[n][0] /= this->rho[n];
                this->u[n][1] /= this->rho[n];
            }

            this->compute_feq(n);
            for (int i = 0; i < velocities; i++) {
                this->f[n][i] = this->f_eq[n][i] + (1.0f - 1.0f / this->nu) * (this->f[n][i] - this->f_eq[n][i]);
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
    
}

// ---------------------------------------------------------------------------------------------------------


void LBM::streaming() { // Streaming step
    #if defined(SIM_FLUID)

        #if defined(D2Q9)
            //this->f_temp = this->f;

            // Streaming step
            #pragma omp parallel for
            for (int j = Ny - 1; j >= 1; --j) {
                for (int i = 0; i < Nx - 1; ++i) {
                    this->f[i + Nx * j][2] = this->f[i + Nx * (j - 1)][2];
                    this->f[i + Nx * j][6] = this->f[(i + 1) + Nx * (j - 1)][6];
                }
            }
            for (int j = Ny - 1; j >= 1; --j) {
                for (int i = Nx - 1; i >= 1; --i) {
                    this->f[i + Nx * j][1] = this->f[(i - 1) + Nx * j][1];
                    this->f[i + Nx * j][5] = this->f[(i - 1) + Nx * (j - 1)][5];
                }
            }
            for (int j = 0; j < Ny - 1; ++j) {
                for (int i = Nx - 1; i >= 1; --i) {
                    this->f[i + Nx * j][4] = this->f[i + Nx * (j + 1)][4];
                    this->f[i + Nx * j][8] = this->f[(i - 1) + Nx * (j + 1)][8];
                }
            }
            for (int j = 0; j < Ny - 1; ++j) {
                for (int i = 0; i < Nx - 1; ++i) {
                    this->f[i + Nx * j][3] = this->f[(i + 1) + Nx * j][3];
                    this->f[i + Nx * j][7] = this->f[(i + 1) + Nx * (j + 1)][7];
                }
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

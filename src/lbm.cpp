#include "lbm.hpp"


LBM::LBM(const uint Nx, const uint Ny, const uint Nz, const float nu, const uint num_threads) 
    :Nx(Nx), Ny(Ny), Nz(Nz), nu(nu), num_threads(num_threads) { // initialize member variables
}

void LBM::start() {
    print_logo();
    cout << "Starting LBM simulation...\n";
    initialize();
    check_erros();
}; // Run the LBM simulation

void LBM::run(const uint timesteps) {
    uint step = 0u;
    while (step <= timesteps) {
        step++;
        advance_in_time();
    }
}; // Run the LBM simulation


void LBM::initialize() {
    // Initialize the lattice
    const uint N = Nx*Ny*Nz; // Number of lattice cells

    #if defined(D2Q9) && defined(SIM_FLUID)
        // Initialize the distribution functions
        vector<vector<double>> f(N, vector<double>(9, 0.0)); // Distribution functions
        vector<vector<double>> f_eq(N, vector<double>(9, 0.0)); // Distribution functions

        // Initialize the macroscopic variables
        vector<vector<double>> rho(N, vector<double>(1, 0.0)); // Density
        vector<vector<double>> u(N, vector<double>(2, 0.0)); // Velocity
        
    #elif defined(D2Q9) && defined(SIM_PLASMA)
        // Initialize the distribution functions
        vector<vector<double>> f(N, vector<double>(9, 0.0)); // Distribution functions
        vector<vector<double>> f_eq(N, vector<double>(9, 0.0)); // Distribution functions
        vector<vector<double>> g(N, vector<double>(9, 0.0)); // Distribution functions
        vector<vector<double>> g_eq(N, vector<double>(9, 0.0)); // Distribution functions

        // Initialize the macroscopic variables
        vector<vector<double>> rho(N, vector<double>(1, 0.0)); // Density
        vector<vector<double>> u(N, vector<double>(3, 0.0)); // Velocity
        vector<vector<double>> E(N, vector<double>(3, 0.0)); // Electric field
        vector<vector<double>> B(N, vector<double>(3, 0.0)); // Magnetic field
    
    #elif defined(D3Q15) && defined(SIM_FLUID)
        // Initialize the distribution functions
        vector<vector<double>> f(N, vector<double>(15, 0.0)); // Distribution functions
        vector<vector<double>> f_eq(N, vector<double>(15, 0.0)); // Distribution functions

        // Initialize the macroscopic variables
        vector<vector<double>> rho(N, vector<double>(1, 0.0)); // Density
        vector<vector<double>> u(N, vector<double>(3, 0.0)); // Velocity

    #elif defined(D3Q15) && defined(SIM_PLASMA)
        // Initialize the distribution functions
        vector<vector<double>> f(N, vector<double>(15, 0.0)); // Distribution functions
        vector<vector<double>> f_eq(N, vector<double>(15, 0.0)); // Distribution functions
        vector<vector<double>> g(N, vector<double>(15, 0.0)); // Distribution functions
        vector<vector<double>> g_eq(N, vector<double>(15, 0.0)); // Distribution functions

        // Initialize the macroscopic variables
        vector<vector<double>> rho(N, vector<double>(1, 0.0)); // Density
        vector<vector<double>> u(N, vector<double>(3, 0.0)); // Velocity
        vector<vector<double>> E(N, vector<double>(3, 0.0)); // Electric field
        vector<vector<double>> B(N, vector<double>(3, 0.0)); // Magnetic field

    #elif defined(D3Q19) && defined(SIM_FLUID)
        // Initialize the distribution functions
        vector<vector<double>> f(N, vector<double>(19, 0.0)); // Distribution functions
        vector<vector<double>> f_eq(N, vector<double>(19, 0.0)); // Distribution functions

        // Initialize the macroscopic variables
        vector<vector<double>> rho(N, vector<double>(1, 0.0)); // Density
        vector<vector<double>> u(N, vector<double>(3, 0.0)); // Velocity
    
    #elif defined(D3Q19) && defined(SIM_PLASMA)
        // Initialize the distribution functions
        vector<vector<double>> f(N, vector<double>(19, 0.0)); // Distribution functions
        vector<vector<double>> f_eq(N, vector<double>(19, 0.0)); // Distribution functions
        vector<vector<double>> g(N, vector<double>(19, 0.0)); // Distribution functions
        vector<vector<double>> g_eq(N, vector<double>(19, 0.0)); // Distribution functions

        // Initialize the macroscopic variables
        vector<vector<double>> rho(N, vector<double>(1, 0.0)); // Density
        vector<vector<double>> u(N, vector<double>(3, 0.0)); // Velocity
        vector<vector<double>> E(N, vector<double>(3, 0.0)); // Electric field
        vector<vector<double>> B(N, vector<double>(3, 0.0)); // Magnetic field

    #elif defined(D3Q27) && defined(SIM_FLUID)
        // Initialize the distribution functions
        vector<vector<double>> f(N, vector<double>(27, 0.0)); // Distribution functions
        vector<vector<double>> f_eq(N, vector<double>(27, 0.0)); // Distribution functions

        // Initialize the macroscopic variables
        vector<vector<double>> rho(N, vector<double>(1, 0.0)); // Density
        vector<vector<double>> u(N, vector<double>(3, 0.0)); // Velocity
    
    #elif defined(D3Q27) && defined(SIM_PLASMA)
        // Initialize the distribution functions
        vector<vector<double>> f(N, vector<double>(27, 0.0)); // Distribution functions
        vector<vector<double>> f_eq(N, vector<double>(27, 0.0)); // Distribution functions
        vector<vector<double>> g(N, vector<double>(27, 0.0)); // Distribution functions
        vector<vector<double>> g_eq(N, vector<double>(27, 0.0)); // Distribution functions

        // Initialize the macroscopic variables
        vector<vector<double>> rho(N, vector<double>(1, 0.0)); // Density
        vector<vector<double>> u(N, vector<double>(3, 0.0)); // Velocity
        vector<vector<double>> E(N, vector<double>(3, 0.0)); // Electric field
        vector<vector<double>> B(N, vector<double>(3, 0.0)); // Magnetic field

    #else
        std::cerr << "Error: Lattice type not defined. Please define a lattice type in the definitions.hpp file." << std::endl;
        return;
        
    #endif

    
    
    
    // Initialize the boundary conditions
    // Initialize the collision operator
    // Initialize the streaming operator
    // Initialize the equilibrium distribution
    // Initialize the lattice weights
    // Initialize the relaxation time
    // Initialize the lattice velocities
    // Initialize the lattice boundaries
    // Initialize the lattice geometry
    // Initialize the lattice obstacles
    // Initialize the lattice forces
    // Initialize the lattice source terms
    // Initialize the lattice moments

    // Set the initialized flag to true
    initialized = true;

};

void LBM::advance_in_time() { // Perform one LBM time step -> collision -> boundary conditions -> streaming
    this->collision();
    this->boundary_conditions();
    this->streaming();
};

void LBM::collision() {              // Collision step

}; 

void LBM::boundary_conditions() {    // Boundary conditions

};

void LBM::streaming() {              // Streaming step

};  


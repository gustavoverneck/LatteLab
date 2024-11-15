#include "lbm.hpp"


LBM::LBM(const uint Nx, const uint Ny, const uint Nz, const float nu, const uint num_threads) 
    :Nx(Nx), Ny(Ny), Nz(Nz), nu(nu), num_threads(num_threads) {} // initialize member variables


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
    cout << "Initializing variables...\n";
    const uint N = Nx*Ny*Nz; // Number of lattice cells

    #if defined(SIM_FLUID)
        this->f = vector<vector<double>> (N, vector<double>(velocities, 0.0f)); // Distribution functions
        this->f_eq = vector<vector<double>> (N, vector<double>(velocities, 0.0f)); // Distribution functions
        this->rho = vector<double> (N, 0.0f); // Density
        this->u = vector<vector<double>> (N, vector<double>(dimensions, 0.0f)); // Velocity

    #elif defined(SIM_PLASMA)
        this->f = vector<vector<double>> (N, vector<double>(velocities, 0.0f)); // Distribution functions
        this->f_eq = vector<vector<double>> (N, vector<double>(velocities, 0.0f)); // Distribution functions
        this->g = vector<vector<double>> (N, vector<double>(velocities, 0.0f)); // Distribution functions
        this->g_eq = vector<vector<double>> (N, vector<double>(velocities, 0.0f)); // Distribution functions
        this->rho = vector<double> (N, 0.0f); // Density
        this->u = vector<vector<double>> (N, vector<double>(dimensions, 0.0f)); // Velocity
        this->E = vector<vector<double>> (N, vector<double>(dimensions, 0.0f)); // Electric field
        this->B = vector<vector<double>> (N, vector<double>(dimensions, 0.0f)); // Magnetic field

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


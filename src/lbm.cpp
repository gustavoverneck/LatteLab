#include "lbm.hpp"


LBM::LBM(const uint Nx, const uint Ny, const uint Nz, const float nu ) 
    :Nx(Nx), Ny(Ny), Nz(Nz), nu(nu) { // initialize member variables
}

void LBM::run(const uint timesteps) {
    check_erros();
    initialize();
    while (step <= timesteps) {
        step++;
        advance_in_time();
    }
}; // Run the LBM simulation

void LBM::initialize() {
    // Add initialization code here
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


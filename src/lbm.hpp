#pragma once


#include "definitions.hpp"
#include "units.hpp"



class LBM { // Lattice Boltzmann Method class

    private:
        ulong N; // Number of lattice cells
        uint Nx=1u, Ny=1u, Nz=1u; // (global) lattice dimensions
        float nu = 0.1f; // kinematic viscosity
        bool initialized = false; // Defines if the LBM object has been initialized
        // Add a function to verify errors and warnings on initialization

    public:                
        LBM(const uint Nx, const uint Ny, const uint Nz, const float nu); // Constructor

        // Getters
        uint get_Nx() { return Nx; }
        uint get_Ny() { return Ny; }
        uint get_Nz() { return Nz; }
        float get_nu() { return nu; }


        // Setters

        void check_erros() { // Check for errors and warnings
            if (!initialized) {
                std::cerr << "LBM object not initialized. Call LBM::initialize() before calling this function." << std::endl;
                return;
            };
        #ifdef D2Q9
            if(Nz!=1u) std::cerr << "D2Q9 is the 2D velocity set. You have to set Nz=1u in the LBM constructor! Currently you have set Nz="+to_string(Nz)+"u." << std::endl;
        #endif // D2Q9
        };  // check_erros()


        void initialize();  // Initialize the LBM object


        void advance_in_time(); // Perform one LBM time step -> collision -> boundary conditions -> streaming
};

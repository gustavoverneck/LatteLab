#pragma once


#include "definitions.hpp"
#include "units.hpp"
#include "utilities.hpp"



class LBM { // Lattice Boltzmann Method class

    private:
        ulong N=1u; // Number of lattice cells
        uint Nx=1u, Ny=1u, Nz=1u; // (global) lattice dimensions
        uint step = 0u; // Current time step
        float nu = 0.1f; // kinematic viscosity
        bool initialized = false; // Defines if the LBM object has been initialized
        unsigned int num_threads = std::thread::hardware_concurrency(); // Number of threads to be used in the simulation

        void evolve(); // Perform one LBM time step -> collision -> boundary conditions -> streaming

        void collision(); // Collision step

        void boundary_conditions(); // Boundary conditions

        void streaming(); // Streaming step

        void collision_task(int start, int end); // Collision step task

        void streaming_task(int start, int end); // Streaming step task

        void check_erros(); // Check for errors and warnings

    public:                
        LBM(const uint Nx, const uint Ny, const uint Nz, const float nu, const uint num_threads); // Constructor

        // Getters
        uint get_Nx() { return Nx; }
        uint get_Ny() { return Ny; }
        uint get_Nz() { return Nz; }
        float get_nu() { return nu; }
        ulong get_N() { return Nx*Ny*Nz;}
        uint get_num_threads() { return num_threads; }
        uint get_step() { return step; }


        // Setters
        void start(); // Start the LBM simulation

        void run(const uint timesteps); // Run the LBM simulation

        void export_data(); // Export data to a file

        // Constructors of variables
        vector<vector<double>> f; // Distribution functions
        vector<vector<double>> f_eq; // Distribution functions
        vector<vector<double>> f_temp; // Distribution functions (temporary)
        vector<double> rho; // Density
        vector<vector<double>> u; // Velocity
        vector<vector<double>> g; // Distribution functions
        vector<vector<double>> g_eq; // Distribution functions
        vector<vector<double>> E; // Electric field
        vector<vector<double>> B; // Magnetic field
        vector<uint> flags; // Flags for each cell
};

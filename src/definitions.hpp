#pragma once

/*
To select any of the following simulation types or setting, uncomment the corresponding line
*/

/*
    Threads settings

    USE_ALL_THREADS Use all available threads
    THREADS Use a fixed number of threads (is overriden by ALL_THREADS)
    # WARNING: if neither is defined, the simulation will run in a single thread
*/
    //#define USE_ALL_THREADS // Use all available threads
    #define THREADS 8 // Use a fixed number of threads (is overriden by ALL_THREADS)

    
/*
    Simulation type: SIM_FLUID or SIM_PLASMA
*/
    #define SIM_FLUID
    //#define SIM_PLASMA


/*
Grid size and velocities
*/
    #define D2Q9 // In progress
    //#define D3Q15 // Not implemented
    //#define D3Q19 // Not implemented
    //#define D3Q27 // Not implemented


/*
Collision Operator type
*/
    #define BGK // In progress
    //#define TRT // Not implemented
    //#define MRT // Not implemented


/*
Simulation Properties
*/
//#define TEMPERATURE // enables temperature simulation // not implemented
//#define MOVING_BOUNDARIES // enables moving boundaries // not implemented
//#define MULTIPHASE // enables multiphase simulation // not implemented
//#define FORCE_FIELD // enables force field simulation // not implemented

/*
Graphical visualization
*/
//#define GRAPHICAL // enables graphical visualization // not implemented


// -----------------------------------------------------------------------------------------------

// Other definitions

// Types definition to bitwise operations
#define     TYPE_S      0b00000001  // solid boundary
#define     TYPE_IN     0b00000010  // inflow boundary
#define     TYPE_OUT    0b00000100  // equilibrium boundary (inflow/outflow)
#define     TYPE_T      0b00001000  // temperature boundary
#define     TYPE_F      0b00010000  // fluid cell
#define     TYPE_I      0b00100000  // interface cell
#define     TYPE_G      0b01000000  // gas cell
#define     TYPE_P      0b10000000  // plasma cell




// ---------------------------------------------------------------------------------------------------------

// Typedefs
typedef unsigned int uint;
typedef unsigned long ulong;


// -----------------------------------------------------------------------------------------------

// Define the velocities and dimensions for each lattice type
#if defined(D2Q9)
    #define velocities 9
    #define dimensions 2
    #define dt 1.0f // Time step
    // Lattice velocity set
    
    // Lattice weights
    #define def_w0 (1.0f/2.25f)     // center (0)
	#define def_ws (1.0f/9.0f)      // straight (1-4)
	#define def_we (1.0f/36.0f)     // edge (5-8)
    const double w[velocities] = {def_w0, def_ws, def_ws, def_ws, def_ws, def_we, def_we, def_we, def_we};  // Weights

#elif defined(D3Q15)
    const uint velocities = 15;
    const uint dimensions = 3;
    // Lattice weights
    #define def_w0 (1.0f/4.5f)      // center (0)
	#define def_ws (1.0f/9.0f)      // straight (1-6)
	#define def_wc (1.0f/72.0f)     // corner (7-14)
    #define def_w {def_w0, def_ws, def_ws, def_ws, def_ws, def_ws, def_ws, def_we, def_we, def_we, def_we, def_we, def_we, def_we, def_we}

#elif defined(D3Q19)
    const uint velocities = 19;
    const uint dimensions = 3;
    // Lattice weights
    #define def_w0 (1.0f/3.0f)      // center (0)
	#define def_ws (1.0f/18.0f)     // straight (1-6)
	#define def_we (1.0f/36.0f)     // edge (7-18)
    #define def_w {def_w0, def_ws, def_ws, def_ws, def_ws, def_ws, def_ws, def_we, def_we, def_we, def_we, def_we, def_we, def_we, def_we, def_we, def_we, def_we, def_we}

#elif defined(D3Q27)
    const uint velocities = 27;
    const uint dimensions = 3;
    // Lattice weights
    #define def_w0 (1.0f/3.375f)    // center (0)
	#define def_ws (1.0f/13.5f)     // straight (1-6)
	#define def_we (1.0f/54.0f)     // edge (7-18)
	#define def_wc (1.0f/216.0f)    // corner (19-26)
    #define def_w {def_w0, def_ws, def_ws, def_ws, def_ws, def_ws, def_ws, def_we, def_we, def_we, def_we, def_we, def_we, def_we, def_we, def_we, def_we, def_we, def_wc, def_wc, def_wc, def_wc, def_wc, def_wc, def_wc, def_wc, def_wc}

#endif


// Set the number of threads
#ifdef USE_ALL_THREADS
    #ifdef THREADS
        #undef THREADS
    #endif
    #define THREADS omp_get_max_threads()
#elif defined(THREADS) && !defined(ALL_THREADS)
    // Threads defined by the user will be used
#else
    #define THREADS 1
#endif




// -----------------------------------------------------------------------------------------------
// Security checks

// Ensure only one lattice type is defined
#if (defined(D2Q9) + defined(D3Q15) + defined(D3Q19) + defined(D3Q27)) > 1
    #error "Multiple lattice types defined. Please define only one lattice type."
#endif

// Ensure only one collision operator is defined
#if (defined(BGK) + defined(TRT) + defined(MRT)) > 1
    #error "Multiple collision operators defined. Please define only one collision operator."
#endif

// Ensure only one simulation type is defined
#if ((defined(SIM_FLUID) + defined(SIM_PLASMA)) > 1 || (defined(SIM_FLUID) + defined(SIM_PLASMA)) == 0)
    #error "Multiple or no simulation type(s) defined. Please define only one simulation type."
#endif

#pragma once

/*
To select any of the following simulation types or setting, uncomment the corresponding line
*/

/*
    Simulation type: SIM_FLUID or SIM_PLASMA
*/
    //#define SIM_PLASMA
    #define SIM_FLUID


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
#define TYPE_S 0b00000001 // (stationary or moving) solid boundary
#define TYPE_E 0b00000010 // equilibrium boundary (inflow/outflow)
#define TYPE_T 0b00000100 // temperature boundary
#define TYPE_F 0b00001000 // fluid cell
#define TYPE_I 0b00010000 // interface cell
#define TYPE_G 0b00100000 // gas cell
#define TYPE_P 0b01000000 // plasma cell


// Variables definition
typedef unsigned int uint;
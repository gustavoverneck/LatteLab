#pragma once


// Libraries

#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
#include <omp.h>

#include "units.hpp"
// ---------------------------------------------------------------------------------------------------------

// Namespaces
using namespace std;


// ---------------------------------------------------------------------------------------------------------
// Constants
#define pif 3.1415927f
#define pi 3.14159265358979323846
#define dt 1.0f
#define dx 1.0f
#define inf_float as_float(0x7F800000)
#define nan_float as_float(0xFFFFFFFF)
#define inf_double as_double(0x7FF0000000000000)
#define nan_double as_double(0xFFFFFFFFFFFFFFFF)
#define cs 1.0f/sqrt(3.0f)
#define cs2 1.0f/3.0f



// ---------------------------------------------------------------------------------------------------------

// Functions

/** flatten3D
 * @brief Flattens a 3D array into a 1D array.
 * 
 * This function takes a 3D array (vector of vectors of vectors) and flattens it into a single 1D array.
 * 
 * @param array3D The 3D array to be flattened.
 * @return std::vector<int> The resulting 1D array.
 */
inline vector<int> flatten3D(const vector<vector<vector<int>>>& array3D) {
    int profundidade = array3D.size();
    int linhas = array3D[0].size();
    int colunas = array3D[0][0].size();

    std::vector<int> array1D(profundidade * linhas * colunas);

    for (int d = 0; d < profundidade; ++d) {
        for (int l = 0; l < linhas; ++l) {
            for (int c = 0; c < colunas; ++c) {
                int indice = d * (linhas * colunas) + l * colunas + c;
                array1D[indice] = array3D[d][l][c];
            }
        }
    }
    return array1D;
};

// ---------------------------------------------------------------------------------------------------------

/** unflatten1D
 * @brief Converts a 1D array into a 3D array with specified dimensions.
 * 
 * This function takes a 1D array and converts it into a 3D array with the given
 * depth (profundidade), number of rows (linhas), and number of columns (colunas).
 * 
 * @param array1D The input 1D array to be converted.
 * @param profundidade The depth of the resulting 3D array.
 * @param linhas The number of rows in each 2D slice of the resulting 3D array.
 * @param colunas The number of columns in each 2D slice of the resulting 3D array.
 * @return A 3D array with the specified dimensions, containing the elements of the input 1D array.
 */
inline vector<vector<vector<int>>> unflatten1D(const vector<int>& array1D, int profundidade, int linhas, int colunas) {
    vector<vector<vector<int>>> array3D(profundidade, vector<vector<int>>(linhas, vector<int>(colunas)));

    for (int d = 0; d < profundidade; ++d) {
        for (int l = 0; l < linhas; ++l) {
            for (int c = 0; c < colunas; ++c) {
                int indice = d * (linhas * colunas) + l * colunas + c;
                array3D[d][l][c] = array1D[indice];
            }
        }
    }
    return array3D;
};

// ---------------------------------------------------------------------------------------------------------
/** indexToPosition
 * @brief Converts a 1D index to a 3D position.
 *
 * This function takes a 1D index `n` and converts it to a 3D position
 * represented by the coordinates (i, j, k) in a 3D grid with dimensions
 * X, Y, and Z.
 *
 * @param n The 1D index to be converted.
 * @param X The size of the grid in the X dimension.
 * @param Y The size of the grid in the Y dimension.
 * @param Z The size of the grid in the Z dimension.
 * @param i Reference to the variable where the row index (i) will be stored.
 * @param j Reference to the variable where the column index (j) will be stored.
 * @param k Reference to the variable where the depth index (k) will be stored.
 */
inline vector<uint> indexToPosition(uint n, uint Nx, uint Ny, uint Nz) {
    // Convert 1D index n to 3D position (i, j, k)
    return {n / (Ny * Nz), (n / Nz) % Ny, n % Nz};
};

// ---------------------------------------------------------------------------------------------------------
/** positionToIndex
 * @brief Converts a 3D position (i, j, k) to a 1D index.
 * 
 * This function takes a 3D position represented by the coordinates (i, j, k)
 * and converts it to a 1D index based on the dimensions of the 3D space (X, Y, Z).
 * 
 * @param i The x-coordinate in the 3D space.
 * @param j The y-coordinate in the 3D space.
 * @param k The z-coordinate in the 3D space.
 * @param X The total number of elements along the x-axis.
 * @param Y The total number of elements along the y-axis.
 * @param Z The total number of elements along the z-axis.
 * @return The corresponding 1D index.
 */
inline int positionToIndex(vector<uint> v, uint Nx, uint Ny, uint Nz) {
    // Convert 3D position (i, j, k) to 1D index n
    uint i = v[0];
    uint j = v[1];
    uint k = v[2];
    return i * (Ny * Nz) + j * Nz + k;  // Formula to compute 1D index from (i, j, k)
};

// ---------------------------------------------------------------------------------------------------------

/** getNeighbors
 * @brief Get the indices of neighboring cells in a 3D grid.
 * 
 * This function calculates the indices of all valid neighboring cells 
 * around a given cell in a 3D grid. The grid dimensions are specified 
 * by Nx, Ny, and Nz, representing the number of cells in the X, Y, and 
 * Z directions, respectively.
 * 
 * @param n The index of the current cell in the 1D representation of the 3D grid.
 * @param Nx The number of cells in the X direction.
 * @param Ny The number of cells in the Y direction.
 * @param Nz The number of cells in the Z direction.
 * @return std::vector<uint> A vector containing the indices of all valid neighboring cells.
 */
inline vector<uint> getNeighbors(uint n, uint Nx, uint Ny, uint Nz) {
    
    int totalCells = Nx * Ny * Nz;       // Total number of cells
    int layerSize = Ny * Nz;            // Number of elements in one layer (X direction)
    
    // Vector to store all valid neighbors
    std::vector<uint> neighbors;

    // Iterate over the 3x3x3 cube around the current cell
    for (int idx = -1; idx <= 1; ++idx) {
        for (int idy = -1; idy <= 1; ++idy) {
            for (int idz = -1; idz <= 1; ++idz) {
                // Skip the center cell itself
                if (idx == 0 && idy == 0 && idz == 0) continue;

                // Calculate the neighbor index in 1D
                int neighborIndex = n + idx * layerSize + idy * Nz + idz;

                // Check bounds to ensure valid neighbors
                bool inBoundsX = (n / layerSize + idx >= 0) && (n / layerSize + idx < Nx);
                bool inBoundsY = ((n / Nz) % Ny + idy >= 0) && ((n / Nz) % Ny + idy < Ny);
                bool inBoundsZ = (n % Nz + idz >= 0) && (n % Nz + idz < Nz);

                if (inBoundsX && inBoundsY && inBoundsZ) {
                    neighbors.push_back(neighborIndex);
                }
            }
        }
    }
    return neighbors;
};

// ---------------------------------------------------------------------------------------------------------
/** getOpositeDirection
 * @brief Returns the opposite direction index for a given direction index.
 *
 * This function determines the opposite direction index based on the 
 * defined lattice model (D2Q9, D3Q15, D3Q19, or D3Q27). The opposite 
 * direction index is used in lattice Boltzmann methods to identify 
 * the reverse direction of a given direction.
 *
 * @param i The direction index for which the opposite direction is needed.
 * @return The opposite direction index if the input index is valid, 
 *         otherwise returns -1.
 *
 * @note The valid range of the input index depends on the defined lattice model:
 *       - D2Q9: valid indices are 0 to 8
 *       - D3Q15: valid indices are 0 to 14
 *       - D3Q19: valid indices are 0 to 18
 *       - D3Q27: valid indices are 0 to 26
 */
inline int getOpositeDirection(int i) {
    #if defined(D2Q9)   
        switch (i) {
            case 0: return 0;
            case 1: return 3;
            case 2: return 4;
            case 3: return 1;
            case 4: return 2;
            case 5: return 7;
            case 6: return 8;
            case 7: return 5;
            case 8: return 6;
            default: return -1;
        }
    #elif defined(D3Q15)
        switch (i) {
            case 0: return 0;
            case 1: return 3;
            case 2: return 4;
            case 3: return 1;
            case 4: return 2;
            case 5: return 8;
            case 6: return 7;
            case 7: return 6;
            case 8: return 5;
            case 9: return 14;
            case 10: return 13;
            case 11: return 12;
            case 12: return 11;
            case 13: return 10;
            case 14: return 9;
            default: return -1;
        }
    
    #elif defined(D3Q19)
        switch (i) {
            case 0: return 0;
            case 1: return 3;
            case 2: return 4;
            case 3: return 1;
            case 4: return 2;
            case 5: return 8;
            case 6: return 7;
            case 7: return 6;
            case 8: return 5;
            case 9: return 14;
            case 10: return 13;
            case 11: return 12;
            case 12: return 11;
            case 13: return 10;
            case 14: return 9;
            case 15: return 18;
            case 16: return 17;
            case 17: return 16;
            case 18: return 15;
            default: return -1;
        }
    
    #elif defined(D3Q27)
        switch (i) {
            case 0: return 0;
            case 1: return 3;
            case 2: return 4;
            case 3: return 1;
            case 4: return 2;
            case 5: return 8;
            case 6: return 7;
            case 7: return 6;
            case 8: return 5;
            case 9: return 14;
            case 10: return 13;
            case 11: return 12;
            case 12: return 11;
            case 13: return 10;
            case 14: return 9;
            case 15: return 18;
            case 16: return 17;
            case 17: return 16;
            case 18: return 15;
            case 19: return 26;
            case 20: return 25;
            case 21: return 24;
            case 22: return 23;
            case 23: return 22;
            case 24: return 21;
            case 25: return 20;
            case 26: return 19;
            default: return -1;
        }

    #endif
};


// ---------------------------------------------------------------------------------------------------------
// Extras

inline void print_logo() {
    std::cout << R"(
-------------------------------------------------------------------------------------------------                                                                                                                                                                                                                                                                                                           
                                :+.                                                                 
                             :%% .%%%%%%#:                                                          
                           #=%% %%%:   .*%%*                                                        
                         #%%*% +%=        .#%*                                                      
                       *%%%*@# %%           .%%:   -%%#                                             
                     #@%%%%@@= %%             #%=  %%%%.                                            
                   #@@@%%%@@@- @%              +%+                                                  
                 %@@@@@@@@@@@* %@               +%:                                                 
               #@@@@@@@@@@@@@% +@.              .%%.                                                
              -@@@@@@@@@@@@@@@..@@               .#*                                                
              #@@@@@@@@@@@@@@@@ +@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@=  #@@@@-                       
              #@@@@@@@@@@@@@@@@= %@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@+  *@@@@:                       
              =@@@@@@@@@@@@@@@@@- %@@@@@@@@@@@@@@@@@@@@@@@@@                                        
               @@@@@@@@@@@@@@%%%@- %@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@               
               :@@@@@@@@@@@@%@@@@@* -@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@+              
                +@@@@@@@@@@%%@@@@@@@. #@@@@@@@@@**++++++*++++++++++++++++++++++++++=                
                 .@@@@@@@@@@@@@@@@@@@#  *@@@@@@+    .....                                           
                   %@@@@@@@@@@@@@@@@@@@%:  =#*.  *@@@@@@@@-                                         
                     +@@@@@@@@@@@@@@@@@@@@%+.   -**######-                                          
                                                                                                    
                                                                                                    
                                                               =                    #               
     :@@#                     @@@.     @@@                     @.                   @.              
     :@@#           +%#=     .@@@+=-   @@@-=     -#%*          @.          :=-      @.  ==.         
     :@@#         @@@%%@@@. +@@@@@@: @@@@@@@.  @@@##@@@.       @.        @=   .@.   @*#    #@       
     :@@#              .@@%   %@@      @@@    @@%    %@@       @.              +#   @-      %=      
     :@@*        :@@@@%%@@@   @@@      @@@    @@@@@@@@@@       @.        *@#*++%#   @.      :%      
     :@@#        @@@   .@@@   @@@      @@@    %@@-   .=.       @.       @:     *#   @+      @-      
     :@@@@@@@@@# -@@@@@@@@@   -@@@@@   #@@@@=  +@@@@@@@.       @*=====- :@.  -@+%   @:@=. +@-       
                                                                                                                                                                                                                                                                                                            
Gustavo A. Verneck, 2024.
-------------------------------------------------------------------------------------------------                                                                                                                                                                                                                                                                                                           
)" << std::endl;
};
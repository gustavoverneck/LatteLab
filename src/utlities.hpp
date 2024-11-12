#pragma once


// Libraries

#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>

// ---------------------------------------------------------------------------------------------------------

// Namespaces
using namespace std;


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
std::vector<int> flatten3D(const std::vector<std::vector<std::vector<int>>>& array3D) {
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
}

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
std::vector<std::vector<std::vector<int>>> unflatten1D(const std::vector<int>& array1D, int profundidade, int linhas, int colunas) {
    std::vector<std::vector<std::vector<int>>> array3D(profundidade, std::vector<std::vector<int>>(linhas, std::vector<int>(colunas)));

    for (int d = 0; d < profundidade; ++d) {
        for (int l = 0; l < linhas; ++l) {
            for (int c = 0; c < colunas; ++c) {
                int indice = d * (linhas * colunas) + l * colunas + c;
                array3D[d][l][c] = array1D[indice];
            }
        }
    }
    return array3D;
}

// ---------------------------------------------------------------------------------------------------------
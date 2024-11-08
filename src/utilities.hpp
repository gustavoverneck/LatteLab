#pragma once

// Include standard libraries
#include <iostream> // standard input/output stream objects
#include <vector> // standard vector class
#include <iostream> // standard input/output stream objects

#include <thread> // standard thread class
#include <functional> // standard function class

// Entender melhor o que é cada uma dessas bibliotecas
#include <atomic>
#include <mutex>

// Custom constants definition
#define pi 3.14159265358979323846 // set PI value
#define pif 3.1415927f // set PI float value
#define GRAVITY 9.81 // set gravity value


// Custom types definition
#define unsigned int uint // set unsigned int as uint
#define unsigned char uchar // set unsigned char as uchar
#define unsigned long ulong // set unsigned long as ulong




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
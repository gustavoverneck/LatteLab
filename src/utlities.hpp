#pragma once


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
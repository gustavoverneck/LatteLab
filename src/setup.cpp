#include "setup.hpp"

void main_setup() { // main_setup for the lbm simulation
    const float Nx = 100u;
    const float Ny = 100u;
    const float Nz = 1u;
    const float nu = 0.1f;
    LBM lbm(Nx, Ny, Nz, nu);

    cout << "Nx: " << lbm.Nx() << endl;
    cout << "Ny: " << lbm.get_Ny() << endl;
    cout << "Nz: " << lbm.get_Nz() << endl;
    cout << "nu: " << lbm.get_nu() << endl;
    cout << "N: " << to_string(lbm.get_N()) << endl;
    cout << "dt: " << dt << endl;
    cout << "dx: " << dx << endl;
};

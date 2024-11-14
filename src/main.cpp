#include "lbm.hpp"
#include "setup.hpp"

void main_physics() {
    //cout << "Physics initialized!\n";
    main_setup();
};


int main () {
    //cout << "Simulation initialized!\n";
    main_physics();
    return 0;
};

#include "lbm.hpp"
#include "setup.hpp"

int main () {
    // Set process name
    #if defined(__linux__)
        prctl(PR_SET_NAME, "LatteLab", 0, 0, 0);
    #elif defined(__APPLE__)
        pthread_setname_np("LatteLab");
    #elif defined(_WIN32)
        SetConsoleTitle("LatteLab");
    #else
        #error "Unknown OS"
    #endif
    main_setup();
    cout << "LBM simulation completed.\n";
    return 0;
};

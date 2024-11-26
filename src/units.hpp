#pragma once

#include "utilities.hpp"


inline double nu_from_reynolds(double Re, double u, double L) {
    return u * L / Re;
}
#pragma once

#include <lbm.hpp>

#include "definitions.hpp"
#include "units.hpp"


#if defined (D2Q9)
    const uint velocities = 9;
#elif defined (D3Q15)
    const uint velocities = 15;
#elif defined (D3Q19)
    const uint velocities = 19;
#elif defined (D3Q27)
    const uint velocities = 27;
#endif


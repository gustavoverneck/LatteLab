#pragma once

// Space definition
#define D2Q9
#define D3Q19

//#define PALALLELIZE // uncomment to enable parallelization
//#define GPU_ACCELERATION // uncomment to enable GPU acceleration

#define GRAPHICS
#ifdef GRAPHICS
    // Window properties definition
    #define GRAPHICS_WINDOW_WIDTH 1600 // set window width
    #define GRAPHICS_WINDOW_HEIGHT 900 // set window height
#endif



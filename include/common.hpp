#pragma once

#include "error_handler.hpp"
#include <cstdio>

namespace img_loader {
    typedef unsigned int uint;
    typedef unsigned short ushort;
    typedef unsigned char byte;

    enum _extension {
        _EXT_UNKNOWN,
        _EXT_BMP,
        _EXT_PNG,
        _EXT_JPG
    };

    struct _color_rgb_quad {
        byte red;
        byte green;
        byte blue;
        byte reserved;
    };

    /// PUBLIC:

    enum pixel_format {
        F_RGB = 0x1907,
        F_RGBA = 0x1908,
        F_BGR = 0x80E0,
        F_BGRA = 0x80E1,
    };

    enum components_layout {
        CL_UBYTE = 0x1401,
        CL_USHORT_5551 = 0x8034,
    };

    // stores image data related info
    struct pixel_info {
        // has same values as in openGL
        pixel_format format;
        // describes layout of components in single pixel
        // has same values as in openGL
        components_layout layout;
    };
}
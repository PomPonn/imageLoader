#pragma once

#include "common.hpp"

namespace img_loader {
    struct _app0_marker {
        ushort version;
        /*
        units = 0:  no units, X and Y specify the pixel aspect ratio
        units = 1:  X and Y are dots per inch
        units = 2:  X and Y are dots per cm
        */
        byte units;
        ushort x_density;
        ushort y_density;
        // some useless stuff... (in this case)
    };

    byte* _load_jpeg_file(FILE* file, int& width, int& height, pixel_info* px_info);
}
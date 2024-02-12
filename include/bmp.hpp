#pragma once

#include "common.hpp"

namespace img_loader {
    struct _bmp_info_header {
        uint header_size;
        long width; // image width
        long height; // image height
        ushort planes; // always 1
        /*
        Bits per pixel:
        1 = monochrome palette,
        4 = 4bit palletized,
        8 = 8bit palletized,
        16 = 16bit RGB,
        24 = 24bit RGB,
        32 = 32bit RGBA
        */
        ushort bit_count;
        /*
        0 = no compression
        1 = 8bit RLE encoding
        2 = 4bit RLE encoding
        */
        uint compression;
        uint image_size; // in bytes
        uint x_pixels_per_m;
        uint y_pixels_per_m;
        // color table related fields:
        uint colors_used;
        uint colors_important; // 0 = all
    };

    struct _bmp_color_table {
        uint size;
        // max 256
        _color_rgb_quad* colors;

        _bmp_color_table() { colors = nullptr; };
    };

    byte* _bmp_decompress_with_color_table(byte* data, _bmp_color_table& ctable, uint image_size, uint bit_count);

    // reads color table memory pointed by file and returns its struct
    _bmp_color_table _bmp_read_color_table(FILE* file, _bmp_info_header& info_header);

    byte* _load_bitmap(FILE* file, int& width, int& height, pixel_info* px_info);
}
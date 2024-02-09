#pragma once

#include "common.hpp"

namespace img_loader {
    // only critical chunks for now
    enum _png_chunk_type {
        PNG_IHDR = 0x49484452, // image header
        PNG_IDAT = 0x49444154, // image data
        PNG_IEND = 0x49454E44, // image end marker
        PNG_PLTE = 0x504C5445, // image palette
    };
    struct _png_chunk {
        uint data_length;
        uint type;
        void* data;
        uint CRC; // Cyclic Redundancy Check

        //bool isEmpty() { return data == nullptr; }
    };
    struct _png_header_chunk_data {
        uint img_width;
        uint img_height;
        byte bit_depth;
        byte color_type;
        // compression method
        byte compr_method;
        byte filter_method;
        byte interlace_method;

        _png_header_chunk_data(const byte* data);
    };
    struct _png_palette_chunk_data {
        // contains from 1 to 256 entries
        // 3 bytes per entry -> R G B
        byte* entries;

        _png_palette_chunk_data(const byte* data);
    };
    struct _png_data_chunk_data {
        byte* compressed_data;

        _png_data_chunk_data(const byte* data);
    };

    _png_chunk* _load_png_chunk(const byte* data);

    byte* _load_png_file(FILE* file, int& width, int& height, pixel_info* format_info);
}
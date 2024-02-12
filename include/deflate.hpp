#pragma once

#include "common.hpp"

namespace img_loader {

    // contains deflate decompression related tools
    class deflate {
        struct _zlib_datastream {
            /*
            bits 0 - 3
            compression method - always 8 in case of PNG.
            bits 4 - 7
            compression info - base-2 logarithm of the LZ77 window
            size, minus eight. Values mustn't be above 7.
            */
            byte compression;
            /*
            bits 0 - 4
            check bits - value must be such that CMF and FLG, when viewed as
            a 16-bit unsigned integer stored in MSB order (CMF*256 + FLG),
            is a multiple of 31
            bit 5
            preset dictionary flag - must be 0 for PNG.
            bits 6 - 7
            (no need to read)
            compression level - 0 - fastest algorithm
                                1 - fast algorithm
                                2 - default algorithm
                                3 - maximum compression, slowest algorithm
            */
            byte flags;

            // no dictionary id here, since it will never be present

            /*compressed data*/

            uint adler32;
        };
    };
}
#pragma once

namespace img_loader {
    typedef unsigned char uword_t;
    typedef unsigned char byte_t;

    /// <COMMON> ///

    enum _extension {
        _EXT_UNKNOWN,
        _EXT_BMP,
        _EXT_PNG,
        _EXT_JPG
    };

    static _extension _get_extension(const char* filename);

    /// </COMMON> ///

    /// <PNG> ///

    // only critical chunks
    enum _png_chunk_type {
        PNG_IHDR = 0x49484452, // image header
        PNG_IDAT = 0x49444154, // image data
        PNG_IEND = 0x49454E44, // image end marker
        PNG_PLTE = 0x504C5445, // image palette
    };
    struct _png_chunk {
        uword_t data_length;
        uword_t type;
        void* data;
        uword_t CRC; // Cyclic Redundancy Check

        bool isEmpty() { return data == nullptr; }
    };
    struct _png_header_chunk_data {
        uword_t img_width;
        uword_t img_height;
        byte_t bit_depth;
        byte_t color_type;
        // compression method
        byte_t compr_method;
        byte_t filter_method;
        byte_t interlace_method;

        _png_header_chunk_data(const byte_t* data);
    };
    struct _png_palette_chunk_data {
        // contains from 1 to 256 entries
        // 3 bytes per entry -> R G B
        byte_t* entries;

        _png_palette_chunk_data(const byte_t* data);
    };
    struct _png_data_chunk_data {
        byte_t* compressed_data;

        _png_data_chunk_data(const byte_t* data);
    };

    static _png_chunk* _load_png_chunk(const byte_t* data);

    /// </PNG> ///

    /// <BMP> ///



    /// </BMP> ///

    /// <PUBLIC> ///

    static unsigned char* load(const char* file_path, int& width, int& height);

    /// </PUBLIC> ///
};
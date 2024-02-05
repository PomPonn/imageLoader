#pragma once

namespace img_loader {
    typedef unsigned int uint;
    typedef unsigned short ushort;
    typedef unsigned char byte;

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
        uint data_length;
        uint type;
        void* data;
        uint CRC; // Cyclic Redundancy Check

        bool isEmpty() { return data == nullptr; }
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

    /// </PNG> ///

    /// <BMP> ///

    struct _bmp_info_header {
        uint header_size; // 40
        uint width; // image width
        uint height; // image height
        ushort planes; // 1
        /*
        Bits per pixel
        1 = monochrome palette, num_colors = 1
        4 = 4bit palletized, num_colors = 16
        8 = 8bit palletized, num_colors = 256
        16 = 16bit RGB, num_colors = 56636 (?)
        24 = 24bit RGB, num_colors = 16M
        */
        ushort bit_count;
        /*
        0 = BI_RGB - no compression
        1 = BI_RLE8 - 8bit RLE encoding
        2 = BI_RLE4 - 4bit RLE encoding
        */
        uint compression;
        uint image_size;
        uint x_pixels_per_m;
        uint y_pixels_per_m;
        // palette related information
        uint colors_used;
        uint colors_important; // 0 = all
    };
    // present only when bit_count <= 8
    struct _bmp_color_table {
        ushort size; // always bit_count ^ 2
        // 1 byte for each component - R G B and 1 reserved byte
        uint* colors;
    };

    /// </BMP> ///

    /// <PUBLIC> ///

    /// @brief loads given file into array of bytes
    /// @param file_path path to the file
    /// @param width returns width of the file
    /// @param height returns height of the file
    /// @return pointer to the raw image data
    byte* load(const char* file_path, int& width, int& height);

    /// </PUBLIC> ///
};
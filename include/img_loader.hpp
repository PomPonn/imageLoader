#pragma once

#include <fstream>

namespace img_loader {
    typedef unsigned int uint;
    typedef unsigned short ushort;
    typedef unsigned char byte;

    struct image_data;

    /// <COMMON> ///

    enum _extension {
        _EXT_UNKNOWN,
        _EXT_BMP,
        _EXT_PNG,
        _EXT_JPG
    };

    _extension _get_extension(const char* filename);

    // returns pointer to read memory block
    byte* _read_file_block(std::ifstream& file, std::streamsize count);

    /// </COMMON> ///

    /// <PNG> ///

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

    /// </PNG> ///

    /// <BMP> ///

    struct cie_xyz {
        long x, y, z;
    };
    struct cie_xyz_rgb {
        cie_xyz red, green, blue;
    };
    struct rgb_quad {
        byte red, green, blue, reserved;
    };

    // when size = 40
    struct _bmp_info_header {
        long width; // image width
        long height; // image height
        ushort planes; // 1
        /*
        Bits per pixel
        1 = monochrome palette, num_colors = 1
        4 = 4bit palletized, num_colors = 16
        8 = 8bit palletized, num_colors = 256
        16 = 16bit RGB, num_colors = 56636 (?)
        24 = 24bit RGB, num_colors = 16M
        ...
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

    // when size = 124
    struct _bmp_v5_header {
        long width;
        long height;
        ushort planes;
        ushort bit_count;
        uint compression;
        uint image_size;
        uint x_pixels_per_m;
        uint y_pixels_per_m;
        uint colors_used;
        uint colors_important;
        uint red_mask;
        uint green_mask;
        uint blue_mask;
        uint alpha_mask;
        uint color_space_type;
        cie_xyz_rgb endpoints;
        uint gamma_red;
        uint gamma_green;
        uint gamma_blue;
        uint intent;
        uint profile_data;
        uint profile_size;
        uint reserved;
    };
    // present only when bit_count <= 8 and compression == 0
    struct _bmp_color_table {
        uint size;
        rgb_quad* colors;
    };
    // when compression == 3
    struct _bmp_color_masks {
        uint red, green, blue;
    };

    // alters img_data
    void _bmp_decompress_with_color_table
    (byte* indexes, image_data& img_data, _bmp_color_table& ctable);

    // alters img_data
    void _bmp_handle_color_table(std::ifstream& file, uint colors_used, image_data& img_data);

    // alters img_data
    void _bmp_set_components_info(image_data& img_data);

    void _load_bitmap(std::ifstream& file, image_data& img_data);

    /// </BMP> ///

    /// <PUBLIC> ///

    // contains same values as in openGL
    enum pixel_format {
        F_RGB = 0x1907,
        F_RGBA = 0x1908,
        F_BGR = 0x80E0,
        F_BGRA = 0x80E1,
    };

    // contains same values as in openGL
    enum components_layout {
        CL_UBYTE = 0x1401,
        CL_USHORT_5551 = 0x8034,
    };

    // stores image data related info
    struct image_data {
        byte* array;
        uint width;
        uint height;
        // actual byte size of the image
        uint size;
        uint bit_depth;
        pixel_format format;
        components_layout layout;

        image_data();

        bool isValid();
    };

    /// @brief loads given file data
    /// @param file_path path to the file
    /// @return pointer to the image data
    image_data load(const char* file_path);

    /// @brief frees image memory
    /// @param data pointer to image data
    void free(image_data& data);

    /// </PUBLIC> ///
};
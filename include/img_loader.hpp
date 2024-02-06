#pragma once

#include <fstream>

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

    _extension _get_extension(const char* filename);

    /// @brief trims high byte of every pixel
    /// @param data pointer to the image data
    /// @param size image data size
    /// @return pointer to new image data
    byte* _trim_high_byte(byte* data, uint bit_count, size_t size, size_t& new_size);

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

    byte* _load_bitmap(std::ifstream& file, int& width, int& height);

    /// </BMP> ///

    /// <PUBLIC> ///

    // TODO: implement this
    // stores data related info like pixel format, ...
    struct image_data_info {

    };

    /// @brief loads given file data
    /// @param file_path path to the file
    /// @param width returns width of the file
    /// @param height returns height of the file
    /// @return pointer to the image data
    byte* load(const char* file_path, int& width, int& height);

    /// @brief frees image memory
    /// @param data pointer to image data
    void free(byte* data);

    /// </PUBLIC> ///
};
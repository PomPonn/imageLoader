#include "bmp.hpp"

#include <cmath>    // pow()

namespace img_loader {
    byte* _load_bitmap(FILE* file, int& width, int& height, pixel_info* format_info) {
        // skip file header, since its unnecessary to read
        fseek(file, 14, SEEK_SET);

        // read info header
        _bmp_info_header info_header;
        fread(&info_header, sizeof(info_header), 1, file);

        width = info_header.width;
        height = info_header.height;

        // calculate (un)compressed image size (in bytes)
        uint image_size = info_header.image_size ? info_header.image_size
            : width * height * (info_header.bit_count / 8);

        fseek(file, info_header.header_size + 14, SEEK_SET);

        // check if next is color table
        if (info_header.bit_count <= 8) {
            _bmp_color_table color_table = _bmp_read_color_table(file, info_header);

            switch (info_header.compression)
            {
            case 0: { // uncompressed

                if (format_info) {
                    format_info->format = F_RGB;
                    format_info->layout = CL_UBYTE;
                }

                // read indexes of color table entries
                byte* data = new byte[image_size];
                fread(data, sizeof(byte), image_size, file);

                fclose(file);
                return _bmp_decompress_with_color_table(data, color_table, image_size, info_header.bit_count);
            }
            case 1: // RLE8
            case 2: // RLE4
                _set_error(ERR_BMP_RLE);
                break;
            default:
                _set_error(ERR_BMP_STRUCT);
                break;
            }
        }
        // uncompressed 8bpp+ data
        else if (info_header.compression == 0) {
            if (format_info) {
                if (info_header.bit_count == 32) {
                    format_info->format = F_BGRA;
                }
                else {
                    format_info->format = F_BGR;
                }

                if (info_header.bit_count == 16) {
                    format_info->layout = CL_USHORT_5551;
                }
                else {
                    format_info->layout = CL_UBYTE;
                }
            }
            byte* data = new byte[image_size];
            fread(data, sizeof(byte), image_size, file);

            fclose(file);
            return data;
        }
        // bit fields
        else {
            _set_error(ERR_BMP_BITFIELD);
        }

        fclose(file);
        return nullptr;
    }

    _bmp_color_table _bmp_read_color_table(FILE* file, _bmp_info_header& info_header) {
        _bmp_color_table color_table;

        if (!info_header.colors_used) {
            color_table.size = (uint)pow(2, info_header.bit_count);
        }
        else {
            color_table.size = info_header.colors_used;
        }

        // read color table
        color_table.colors = new _color_rgb_quad[color_table.size];
        fread(color_table.colors, sizeof(_color_rgb_quad), color_table.size, file);
        return color_table;
    }

    //frees every heap allocated memory in parameters
    byte* _bmp_decompress_with_color_table(byte* data, _bmp_color_table& ctable, uint image_size, uint bit_count) {
        // calculate indexes count based on bpp
        uint indexes_count = image_size * (8 / bit_count);
        // allocate decompressed data
        byte* final_data = new byte[indexes_count * 3];

        ushort indexes_per_byte = indexes_count / image_size;

        uint fdata_curr_pos = 0;
        // represents part of the byte to read
        byte byte_modifier;
        if (indexes_per_byte == 1)
            byte_modifier = 0xFF;
        else
            byte_modifier = indexes_per_byte == 2 ? 0xF : 0x01;

        for (uint i = 0; i < image_size; i++) {
            for (uint j = 0; j < indexes_per_byte; j++) {
                // get part of the byte (one index)
                ushort curr_index = data[i] & byte_modifier;

                final_data[fdata_curr_pos++] = ctable.colors[curr_index].red;
                final_data[fdata_curr_pos++] = ctable.colors[curr_index].green;
                final_data[fdata_curr_pos++] = ctable.colors[curr_index].blue;

                // shift the byte, so the next iteration will get next index
                data[i] = data[i] >> bit_count;
            }
        }

        delete[] data;
        delete[] ctable.colors;
        return final_data;
    }
}
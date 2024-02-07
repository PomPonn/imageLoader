#include "img_loader.hpp"

#include "indian.hpp"

#include <iostream>
#include <cmath>

#define PNG_POINT_NEXT_CHUNK(ptr, data_length) ptr += data_length + 12

namespace img_loader {

    _png_header_chunk_data::_png_header_chunk_data(const byte* data) {
        img_width = *(uint*)reverse_indianness(4, (void*)data);
        img_height = *(uint*)reverse_indianness(4, (void*)(data + 4));
        bit_depth = *(byte*)reverse_indianness(4, (void*)(data + 5));
        color_type = *(byte*)reverse_indianness(4, (void*)(data + 6));
        compr_method = *(byte*)reverse_indianness(4, (void*)(data + 7));
        filter_method = *(byte*)reverse_indianness(4, (void*)(data + 8));
        interlace_method = *(byte*)reverse_indianness(4, (void*)(data + 9));
    }

    _png_palette_chunk_data::_png_palette_chunk_data(const byte* data) {
        entries = (byte*)data;
    }

    _png_data_chunk_data::_png_data_chunk_data(const byte* data) {
        compressed_data = (byte*)data;
    }

    byte* _read_file_block(std::ifstream& file, std::streamsize count) {
        byte* buffer = new byte[count];
        file.read((char*)buffer, count);

        if (file.fail()) {
            return nullptr;
        }

        return buffer;
    }

    image_data::image_data()
        : array(nullptr) {};

    bool image_data::isValid() {
        return array && width && height;
    }

    _png_chunk* _load_png_chunk(const byte* data) {
        if (!data) return nullptr;

        _png_chunk* chunk = new _png_chunk;
        byte* ptr = (byte*)data;

        // start reading chunk
        // (png is using big endian byte order, so we need to convert it back to little endian)
        chunk->data_length = *(uint*)reverse_indianness(4, (void*)ptr);
        ptr += 4;

        chunk->type = *(uint*)reverse_indianness(4, (void*)ptr);
        ptr += 4;
        // 'ptr' must point to the beginning of the chunk data

        switch (chunk->type)
        {
        case PNG_IHDR: {
            _png_header_chunk_data* data = new _png_header_chunk_data(ptr);
            chunk->data = (void*)data;
            break;
        }
        case PNG_PLTE: {
            _png_palette_chunk_data* data = new _png_palette_chunk_data(ptr);
            chunk->data = (void*)data;
            break;
        }
        case PNG_IDAT: {
            _png_data_chunk_data* data = new _png_data_chunk_data(ptr);
            // decompression ???
            break;
        }
        case PNG_IEND: {
            break;
        }
        default: {
            chunk->data = nullptr;
            break;
        }
        }

        chunk->CRC = *(uint*)reverse_indianness(4, (void*)(ptr + chunk->data_length));
        return chunk;
    }

    void _bmp_handle_color_table(std::ifstream& file, uint colors_used, image_data& img_data) {
        _bmp_color_table color_table;

        if (colors_used == 0) {
            color_table.size = (uint)pow(2, img_data.bit_depth);
        }
        else {
            color_table.size = colors_used;
        }

        // read color table
        byte* buffer = _read_file_block(file, color_table.size * sizeof(rgb_quad));
        color_table.colors = (rgb_quad*)buffer;

        // read compressed data (indexes of color table entries)
        buffer = _read_file_block(file, img_data.size);
        _bmp_decompress_with_color_table(buffer, img_data, color_table);
        delete[] buffer;

        img_data.format = F_RGB;
        img_data.layout = CL_UBYTE;
    }

    void _bmp_set_components_info(image_data& img_data) {
        if (img_data.bit_depth == 32) {
            img_data.format = F_BGRA;
        }
        else {
            img_data.format = F_BGR;
        }

        if (img_data.bit_depth == 16) {
            img_data.layout = CL_USHORT_5551;
        }
        else {
            img_data.layout = CL_UBYTE;
        }
    }

    void _bmp_decompress_with_color_table
    (byte* indexes, image_data& img_data, _bmp_color_table& ctable) {
        // calculate indexes count based on bit depth
        uint indexes_count = img_data.size * (8 / img_data.bit_depth);
        // allocate decompressed data
        byte* final_data = new byte[indexes_count * 3];

        ushort indexes_per_byte = indexes_count / img_data.size;

        uint fdata_curr_pos = 0;
        // represents part of the byte to read
        byte byte_modifier;
        if (indexes_per_byte == 1)
            byte_modifier = 0xFF;
        else
            byte_modifier = indexes_per_byte == 2 ? 0xF : 0x01;

        for (uint i = 0; i < img_data.size; i++) {
            for (uint j = 0; j < indexes_per_byte; j++) {
                // get part of the byte (one index)
                ushort curr_index = indexes[i] & byte_modifier;

                final_data[fdata_curr_pos++] = ctable.colors[curr_index].red;
                final_data[fdata_curr_pos++] = ctable.colors[curr_index].green;
                final_data[fdata_curr_pos++] = ctable.colors[curr_index].blue;

                // shift the byte, so the next iteration will get next index
                indexes[i] = indexes[i] >> img_data.bit_depth;
            }
        }

        img_data.array = final_data;
    }

    void _load_bitmap(std::ifstream& file, image_data& img_data) {
        byte* buffer = nullptr;

        // skip file header, since its unnecessary to read
        file.seekg(10);

        // read data offset
        buffer = _read_file_block(file, 4);
        uint data_offset = *(uint*)buffer;
        delete[] buffer;

        // info header
        if (header_size == 40) {
            // read info header
            buffer = _read_file_block(file, sizeof(_bmp_info_header));
            _bmp_info_header info_header = *(_bmp_info_header*)buffer;
            delete[] buffer;

            img_data.width = info_header.width;
            img_data.height = info_header.height;

            if (info_header.image_size)
                img_data.size = info_header.image_size;
            else
                img_data.size = info_header.width * info_header.height * (info_header.bit_count / 8);

            img_data.bit_depth = info_header.bit_count;

            // color table
            if (info_header.bit_count <= 8 && info_header.compression == 0) {
                _bmp_handle_color_table(file, info_header.colors_used, img_data);
            }
            // uncompressed data
            else if (info_header.compression == 0) {
                _bmp_set_components_info(img_data);

                img_data.array = _read_file_block(file, img_data.size);
            }
            else {
                // bit fields are not supported, since they are rarely used
                std::cerr << "IMG_LOADER::BMP::UNSUPPORTED_FILE_STRUCTURE" << std::endl;
                img_data.array = nullptr;
            }
        }
        // v5 header
        else if (header_size == 124) {
            buffer = _read_file_block(file, sizeof(_bmp_v5_header));
            _bmp_v5_header v5_header = *(_bmp_v5_header*)buffer;
            delete[] buffer;

            img_data.width = v5_header.width;
            img_data.height = v5_header.height;
            img_data.size = img_data.width * img_data.height * (v5_header.bit_count / 8);
            img_data.bit_depth = v5_header.bit_count;

            // color table
            if (v5_header.bit_count <= 8 && v5_header.compression == 0) {
                _bmp_handle_color_table(file, v5_header.colors_used, img_data);
            }
            // uncompressed data
            else if (v5_header.compression == 0) {
                _bmp_set_components_info(img_data);

                img_data.array = _read_file_block(file, img_data.size);
            }
            else {
                // bit fields are not supported, since they are rarely used
                std::cerr << "IMG_LOADER::BMP::UNSUPPORTED_FILE_STRUCTURE" << std::endl;
                img_data.array = nullptr;
            }
        }
        // old headers
        else {
            std::cerr << "IMG_LOADER::BMP::UNSUPPORTED_FILE_STRUCTURE" << std::endl;
            img_data.array = nullptr;
        }
    }

    image_data load(const char* file_path)
    {
        std::ifstream file(file_path, std::ios::binary | std::ios::in);
        image_data img_data;

        if (file.fail()) {
            std::cerr << "IMG_LOADER::OPEN_FILE::FAIL: " << file_path << std::endl;
            return img_data;
        }
        else {
            _extension ext = _get_extension(file_path);

            // if extension is unknown
            if (!ext) {
                std::cerr << "IMG_LOADER::OPEN_FILE::UNKNOWN_FORMAT: " << file_path << std::endl;
                return img_data;
            }

            switch (ext)
            {
            case _EXT_BMP: {
                _load_bitmap(file, img_data);
                break;
            }
            case _EXT_PNG: {
                byte* buffer = nullptr;
                /*
                    FINISH THIS
                */
                // skip PNG marker (8 bytes)
                byte* curr_byte = (byte*)(buffer + 8);

                _png_chunk* IHDR = _load_png_chunk(curr_byte);
                PNG_POINT_NEXT_CHUNK(curr_byte, IHDR->data_length);
                _png_chunk* sec = _load_png_chunk(curr_byte);
                PNG_POINT_NEXT_CHUNK(curr_byte, sec->data_length);
                _png_chunk* th = _load_png_chunk(curr_byte);
                PNG_POINT_NEXT_CHUNK(curr_byte, th->data_length);
                _png_chunk* IDAT = _load_png_chunk(curr_byte);

                break;
            }
            case _EXT_JPG: {
                /*
                    FINISH THIS
                */
                break;
            }
            }
        }

        return img_data;
    }

    void free(image_data& data) {
        delete[] data.array;
    }

    _extension _get_extension(const char* file_path) {
        char* ptr = (char*)file_path;

        while (*ptr++) {}
        while (*ptr != '.') { ptr--; }
        ptr++;

        // now ptr contains extension name
        std::string ext = std::string(ptr);

        if (ext == "jpg" || ext == "jpeg" || ext == "jpe" || ext == "jfif") return _EXT_JPG;
        else if (ext == "png") return _EXT_PNG;
        else if (ext == "bmp" || ext == "dib") return _EXT_BMP;
        else return _EXT_UNKNOWN;
    }

};
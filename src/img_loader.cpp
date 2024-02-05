#include "img_loader.hpp"

#include "indian.hpp"

#include <fstream>
#include <iostream>
#include <cmath>

#define __PNG_POINT_NEXT_CHUNK(ptr, data_length) ptr += data_length + 12

namespace img_loader {

    // struct constructors
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

    byte* load(const char* file_path, int& width, int& height)
    {
        std::ifstream file(file_path, std::ios::binary | std::ios::in);

        if (file.fail()) {
            std::cout << "IMG_LOADER::OPEN_FILE::FAIL -> " << file_path << std::endl;
            return nullptr;
        }
        else {
            _extension ext = _get_extension(file_path);

            // if extension is unknown
            if (!ext) {
                std::cout << "IMG_LOADER::OPEN_FILE::UNSUPPORTED_EXTENSION -> " << file_path << std::endl;
                return nullptr;
            }

            // get file length
            file.seekg(0, file.end);
            size_t length = (size_t)file.tellg();
            file.seekg(0, file.beg);

            // read whole file to the buffer
            unsigned char* buffer = new unsigned char[length];
            file.read((char*)buffer, length);

            byte* curr_byte = nullptr;
            switch (ext)
            {
            case _EXT_BMP: {
                // skip file header, since its unnecessary to read
                curr_byte = (byte*)(buffer + 10);
                // read offset to pixel array after header
                uint data_offset = *(uint*)curr_byte;
                curr_byte += 4;

                _bmp_info_header* info_header = (_bmp_info_header*)curr_byte;
                curr_byte += info_header->header_size;

                width = info_header->width;
                height = info_header->height;

                // check whether color table should appear next
                if (info_header->bit_count <= 8) {
                    _bmp_color_table* color_table = new _bmp_color_table;
                    color_table->size = (ushort)pow(info_header->bit_count, 2);
                    color_table->colors = (uint*)curr_byte;
                    curr_byte += color_table->size;

                    byte* img_data = (byte*)curr_byte;
                }
                else {
                    return (byte*)curr_byte;
                }

                break;
            }
            case _EXT_PNG: {
                /*
                    FINISH THIS
                */
                // skip PNG marker (8 bytes)
                curr_byte = (byte*)(buffer + 8);

                _png_chunk* IHDR = _load_png_chunk(curr_byte);
                __PNG_POINT_NEXT_CHUNK(curr_byte, IHDR->data_length);
                _png_chunk* sec = _load_png_chunk(curr_byte);
                __PNG_POINT_NEXT_CHUNK(curr_byte, sec->data_length);
                _png_chunk* th = _load_png_chunk(curr_byte);
                __PNG_POINT_NEXT_CHUNK(curr_byte, th->data_length);
                _png_chunk* IDAT = _load_png_chunk(curr_byte);

                break;
            }
            case _EXT_JPG: {
                break;
            }
            }
        }

        return nullptr;
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
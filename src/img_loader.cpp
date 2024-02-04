#include "img_loader.hpp"

#include "indian.hpp"

#include <fstream>
#include <iostream>

#define __PNG_POINT_NEXT_CHUNK(ptr, data_length) ptr += data_length + 12

namespace img_loader {

    // struct constructors
    _png_header_chunk_data::_png_header_chunk_data(const byte_t* data) {
        img_width = *(uword_t*)reverse_indianness(4, (void*)data);
        img_height = *(uword_t*)reverse_indianness(4, (void*)(data + 4));
        bit_depth = *(byte_t*)reverse_indianness(4, (void*)(data + 5));
        color_type = *(byte_t*)reverse_indianness(4, (void*)(data + 6));
        compr_method = *(byte_t*)reverse_indianness(4, (void*)(data + 7));
        filter_method = *(byte_t*)reverse_indianness(4, (void*)(data + 8));
        interlace_method = *(byte_t*)reverse_indianness(4, (void*)(data + 9));
    }
    _png_palette_chunk_data::_png_palette_chunk_data(const byte_t* data) {
        entries = (byte_t*)data;
    }
    _png_data_chunk_data::_png_data_chunk_data(const byte_t* data) {
        compressed_data = (byte_t*)data;
    }

    _png_chunk* _load_png_chunk(const byte_t* data) {
        if (!data) return nullptr;

        _png_chunk* chunk = new _png_chunk;
        byte_t* ptr = (byte_t*)data;

        // start reading chunk
        // (png is using big endian byte order, so we need to convert it back to little endian)
        chunk->data_length = *(uword_t*)reverse_indianness(4, (void*)ptr);
        ptr += 4;

        chunk->type = *(uword_t*)reverse_indianness(4, (void*)ptr);
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

        chunk->CRC = *(uword_t*)reverse_indianness(4, (void*)(ptr + chunk->data_length));
        return chunk;
    }

    unsigned char* load(const char* file_path, int& width, int& height)
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

            switch (ext)
            {
            case _EXT_BMP: {
                break;
            }
            case _EXT_PNG: {
                /*
                    FINISH THIS
                */
                // skip PNG marker (8 bytes)
                byte_t* curr_byte = (byte_t*)(buffer + 8);

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

        if (ext == "jpg" || ext == "jpeg" || ext == "jpe" || ext == "jif" || ext == "jfif") return _EXT_JPG;
        else if (ext == "png") return _EXT_PNG;
        else if (ext == "bmp" || ext == "dib") return _EXT_BMP;
        else return _EXT_UNKNOWN;
    }

};
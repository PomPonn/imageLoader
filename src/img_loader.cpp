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

    /**
     * Trims the high byte from a byte array based on the given bit count.
     *
     * @param data The byte array to be trimmed.
     * @param bit_count The number of bits per byte.
     * @param data_size The size of the byte array.
     * @param new_size A reference to a variable that will store the size of the trimmed byte array.
     * @return The trimmed byte array.
     */
    byte* _trim_high_byte(byte* data, uint bit_count, size_t data_size, size_t& new_size) {
        // Check if the input parameters are valid
        if (data_size < 1 || data == nullptr || bit_count < 16 || bit_count % 8 != 0) {
            return nullptr;
        }

        // Calculate the number of bytes per element based on the bit count
        ushort byte_count = bit_count / 8;

        // Calculate the size of the trimmed byte array
        new_size = data_size - data_size / byte_count;

        // Create a new byte array with the calculated size
        byte* result = new byte[new_size];

        // Iterate over the original byte array and copy each element to the new byte array, skipping the elements that will be removed
        size_t result_index = 0;
        for (size_t i = 0; i < data_size; i++) {
            if ((i + 1) % byte_count == 0) {
                continue;
            }
            result[result_index++] = data[i];
        }
        // Delete the original byte array
        delete[] data;

        return result;
    }

    byte* _load_bitmap(std::ifstream& file, int& width, int& height) {
        byte* buffer = nullptr;

        // skip file header, since its unnecessary to read
        file.seekg(10);

        // read image data offset & header size
        buffer = _read_file_block(file, 8);
        uint data_offset = *(uint*)buffer;
        uint header_size = *(uint*)(buffer + 4);
        delete[] buffer;

        // info header
        if (header_size == 40) {
            // read info header
            buffer = _read_file_block(file, sizeof(_bmp_info_header));
            _bmp_info_header info_header = *(_bmp_info_header*)buffer;
            delete[] buffer;

            width = info_header.width;
            height = info_header.height;
            size_t img_size = width * height * (info_header.bit_count / 8);

            // color table
            if (info_header.bit_count <= 8 && info_header.compression == 0) {
                _bmp_color_table color_table;

                if (info_header.colors_used == 0) {
                    color_table.size = (uint)pow(2, info_header.bit_count);
                }
                else {
                    color_table.size = info_header.colors_used;
                }

                buffer = _read_file_block(file, color_table.size * sizeof(rgb_quad));
                color_table.colors = (rgb_quad*)buffer;
                delete[] buffer;

                // TODO: FINISH
                std::cout << "{LOG} IMG_LOADER::color table used";
            }
            // bit fields
            else if (info_header.compression == 3) {
                // read bit fields
                buffer = _read_file_block(file, sizeof(_bmp_color_masks));
                _bmp_color_masks color_masks = *(_bmp_color_masks*)buffer;
                delete[] buffer;

                // TODO: FINISH
                std::cout << "{LOG} IMG_LOADER::bit fields used: "
                    << color_masks.red << " "
                    << color_masks.green << " "
                    << color_masks.blue << std::endl;
            }
            // uncompressed data
            else if (info_header.compression == 0) {
                buffer = _read_file_block(file, img_size);

                // heap error
                if (info_header.bit_count == 32) {
                    size_t trimmed_size = 0;
                    buffer = _trim_high_byte(buffer, info_header.bit_count, img_size, trimmed_size);
                }

                return buffer;
            }
            else {
                std::cerr << "IMG_LOADER::BMP::UNSUPPORTED_FILE_STRUCTURE" << std::endl;
                return nullptr;
            }
        }
        // v5 header
        else if (header_size == 124) {
            buffer = _read_file_block(file, sizeof(_bmp_v5_header));
            _bmp_v5_header v5_header = *(_bmp_v5_header*)buffer;
            delete[] buffer;

            width = v5_header.width;
            height = v5_header.height;
            size_t img_size = width * height * (v5_header.bit_count / 8);

            // uncompressed data
            if (v5_header.compression == 0) {
                buffer = _read_file_block(file, img_size);

                if (v5_header.bit_count == 32) {
                    size_t trimmed_size = 0;
                    buffer = _trim_high_byte(buffer, v5_header.bit_count, img_size, trimmed_size);
                }

                return buffer;
            }
            else {
                //TODO: FINISH
            }
        }
        // old headers
        else {
            std::cerr << "IMG_LOADER::BMP::UNSUPPORTED_FILE_STRUCTURE" << std::endl;
            return nullptr;
        }
    }

    byte* load(const char* file_path, int& width, int& height)
    {
        std::ifstream file(file_path, std::ios::binary | std::ios::in);

        if (file.fail()) {
            std::cerr << "IMG_LOADER::OPEN_FILE::FAIL: " << file_path << std::endl;
            return nullptr;
        }
        else {
            _extension ext = _get_extension(file_path);

            // if extension is unknown
            if (!ext) {
                std::cerr << "IMG_LOADER::OPEN_FILE::UNKNOWN_FORMAT: " << file_path << std::endl;
                return nullptr;
            }

            switch (ext)
            {
            case _EXT_BMP: {
                return _load_bitmap(file, width, height);
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

        return nullptr;
    }

    void free(byte* data) {
        delete[] data;
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
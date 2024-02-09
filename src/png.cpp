#include "png.hpp"

#include "indian.hpp"

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

    byte* _load_png_file(FILE* file, int& width, int& height, pixel_info* format_info) {
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


        return buffer;
    }
}
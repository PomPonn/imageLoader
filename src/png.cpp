#include "png.hpp"

#include "indian.hpp"
#include "inflate.hpp"

#include <vector>

namespace img_loader {

#define _HDR_CAST(chunk_data) ((_png_header_chunk_data*)(chunk_data))
#define _PLTE_CAST(chunk_data) ((_png_palette_chunk_data*)(chunk_data))
#define _IDAT_CAST(chunk_data) ((_png_data_chunk_data*)(chunk_data))

    _png_header_chunk_data::_png_header_chunk_data(byte* data) {
        img_width = *(uint*)reverse_indianness(4, (void*)data);
        img_height = *(uint*)reverse_indianness(4, (void*)(data + 4));
        bit_depth = *(data + 8);
        color_type = *(data + 9);
        compr_method = *(data + 10);
        filter_method = *(data + 11);
        interlace_method = *(data + 12);
    }

    _png_palette_chunk_data::_png_palette_chunk_data(byte* data) {
        entries = data;
    }

    _png_chunk _png_load_chunk(FILE* file) {
        _png_chunk chunk;

        // read chunk data length
        fread(&chunk.data_length, sizeof(chunk.data_length), 1, file);
        chunk.data_length = *(uint*)reverse_indianness(sizeof(chunk.data_length), &chunk.data_length);

        // read chunk type
        fread(&chunk.type, sizeof(chunk.type), 1, file);
        chunk.type = *(uint*)reverse_indianness(sizeof(chunk.type), &chunk.type);

        if (chunk.type) {
            byte* buffer = new byte[chunk.data_length];
            fread(buffer, sizeof(byte), chunk.data_length, file);

            switch (chunk.type)
            {
            case _PNG_IHDR: {
                chunk.data = (void*)new _png_header_chunk_data(buffer);
                delete[] buffer;
                break;
            }
            case _PNG_PLTE: {
                chunk.data = (void*)new _png_palette_chunk_data(buffer);
                break;
            }
            case _PNG_IDAT: {
                chunk.data = (void*)buffer;
                break;
            }
            case _PNG_IEND:
            default: {
                delete[] buffer;
                chunk.data = nullptr;
                break;
            }
            }
        }

        fread(&chunk.CRC, sizeof(chunk.CRC), 1, file);
        chunk.CRC = *(uint*)reverse_indianness(sizeof(chunk.CRC), &chunk.CRC);
        return chunk;
    }

    byte* _load_png_file(FILE* file, int& width, int& height, pixel_info* format_info) {
        // skip PNG marker (8 bytes)
        fseek(file, 8, SEEK_SET);

        _png_chunk header;
        _png_chunk palette{};
        std::vector<_png_chunk> data_chunks;

        header = _png_load_chunk(file);

        // header have to be first chunk
        if (header.type != _PNG_IHDR) {
            _set_error(ERR_FILE_STRUCT);
            return nullptr;
        }

        _png_chunk chunk;
        // process every chunk before image data
        do {
            chunk = _png_load_chunk(file);

            switch (chunk.type)
            {
            case _PNG_PLTE: {
                // check whether palette chunk can appear
                if (_HDR_CAST(header.data)->color_type != 0 && _HDR_CAST(header.data)->color_type != 4) {
                    palette = chunk;
                    break;
                }
                // palette shouldn`t be present
                else {
                    _set_error(ERR_FILE_STRUCT);

                    delete[] header.data;
                    delete[] chunk.data;

                    return nullptr;
                }
            }
            case _PNG_IEND:
                _set_error(ERR_FILE_STRUCT);

                delete[] header.data;
                delete[] palette.data;

                return nullptr;
            case _PNG_IDAT:
                break;
            default: {
                delete[] chunk.data;
                break;
            }
            }
        } while (chunk.type != _PNG_IDAT);

        // get all data chunks
        do {
            data_chunks.push_back(chunk);
            byte* decoded_chunk_data = inflate::decode((byte*)chunk.data, chunk.data_length);

            chunk = _png_load_chunk(file);
        } while (chunk.type == _PNG_IDAT);

        data_chunks.shrink_to_fit();


        auto w = _HDR_CAST(header.data);

        while (data_chunks.size()) {
            chunk = data_chunks.back();


            delete[] chunk.data;
            data_chunks.pop_back();
        }

        delete[] header.data;
        delete[] palette.data;
    }
}
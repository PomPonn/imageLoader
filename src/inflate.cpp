#include "inflate.hpp"

#define _DFL_LOW_BPART(byte) (byte & 0xF)
#define _DFL_HIGH_BPART(byte) ((byte >> 4) & 0xF)
#define _DFL_FLAG_DICT 0b00100000
#define _DFL_FINAL_BMASK 0b00000001
#define _DFL_TYPE_BMASK  0b00000110


namespace img_loader {
    byte* inflate::decode(const byte* compressed_data, uint data_size) {
        _zlib_datastream datastream;
        datastream.compression = compressed_data[0];

        if (_DFL_LOW_BPART(datastream.compression) != 8) {
            _set_error(ERR_FILE_STRUCT);
            return nullptr;
        }

        if (_DFL_HIGH_BPART(datastream.compression) > 7) {
            _set_error(ERR_FILE_STRUCT);
            return nullptr;
        }

        datastream.flags = compressed_data[1];

        // idk what to do with check bits (first 5 bits of flags)

        if (datastream.flags & _DFL_FLAG_DICT) {
            _set_error(ERR_FILE_STRUCT);
            return nullptr;
        }


        bool is_final_block = false;
        uint curr_byte = 2;
        do {
            // read data block header
            is_final_block = compressed_data[curr_byte] & _DFL_FINAL_BMASK;

            /*
                00 - no compression
                01 - compressed with fixed Huffman codes
                10 - compressed with dynamic Huffman codes
                11 - reserved (error)
            */
            byte block_type = (*compressed_data & _DFL_TYPE_BMASK) >> 1;

            switch (block_type) {
            case 0b00: { // no compression

                break;
            }
            case 0b01: { // compressed with fixed Huffman codes

                break;
            }
            case 0b10: { // compressed with dynamic Huffman codes

                break;
            }
            case 0b11: { // reserved (error)

                break;
            }
            }

        } while (!is_final_block);





        return nullptr;
    }
}
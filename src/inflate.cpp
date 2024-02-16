#include "inflate.hpp"

#define _IFL_LOW_BPART(byte) (byte & 0xF)
#define _IFL_HIGH_BPART(byte) (byte >> 4)
#define _IFL_FLAG_DICT 0b00100000
#define _IFL_FINAL_BMASK 0b00000001
#define _IFL_TYPE_BMASK  0b00000110


namespace img_loader {
    byte* inflate::decode(const byte* compressed_data, uint data_size) {
        _zlib_datastream datastream;
        datastream.compression = compressed_data[0];

        if (_IFL_LOW_BPART(datastream.compression) != 8) {
            _set_error(ERR_FILE_STRUCT);
            return nullptr;
        }

        if (_IFL_HIGH_BPART(datastream.compression) > 7) {
            _set_error(ERR_FILE_STRUCT);
            return nullptr;
        }

        datastream.flags = compressed_data[1];

        // idk what to do with check bits (first 5 bits of flags)

        if (datastream.flags & _IFL_FLAG_DICT) {
            _set_error(ERR_FILE_STRUCT);
            return nullptr;
        }


        bool is_final_block = false;
        do {
            // read data block header
            is_final_block = compressed_data[2] & _IFL_FINAL_BMASK;

            /*
                00 - no compression
                01 - compressed with fixed Huffman codes
                10 - compressed with dynamic Huffman codes
                11 - reserved (error)
            */
            byte block_type = (compressed_data[2] & _IFL_TYPE_BMASK) >> 1;

            switch (block_type) {
            case 0b00: { // no compression
                ushort byte_len = *(ushort*)(compressed_data + 3);
                ushort byte_nlen = *(ushort*)(compressed_data + 5);

                break;
            }
            case 0b01: { // compressed with fixed Huffman codes

                break;
            }
            case 0b10: { // compressed with dynamic Huffman codes
                byte literal_codes_num = (compressed_data[2] & 0b11111000) >> 3; // next 5 bits
                byte distance_codes_num = (compressed_data[3] & 0b00011111); // next 5 bits
                byte codelen_codes_num = // next 4 bits
                    (compressed_data[3] & 0b11100000) >> 5
                    | ((compressed_data[4] & 0b00000001) << 3);



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
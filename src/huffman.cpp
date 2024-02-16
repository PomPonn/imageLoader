#include "huffman.hpp"

namespace img_loader {
    huffman::tree* huffman::gen_tree(byte* code_lengths, ushort code_lengths_num, byte codelen_size, byte* next_byte) const {
        uint bit_len = code_lengths_num * codelen_size;

        byte bit_mask;
        switch (codelen_size) {
        case 4:
            bit_mask = 0b00001111;
            break;
        default:
            return nullptr;
        }

        tree huffman_tree;

        uint curr_byte;
        byte code_len;
        // read code lengths
        for (uint bit = 0; bit < bit_len; bit += codelen_size) {
            curr_byte = bit / codelen_size;
            code_len = (code_lengths[curr_byte] >> bit % bit_mask ? 0 : 4) & bit_mask;
        }

        return nullptr;
    }
}
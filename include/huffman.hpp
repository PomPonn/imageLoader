#pragma once

#include "common.hpp"

namespace img_loader {
    class huffman {
        class tree {

        };

        /// @brief generates huffman tree based on the given code lengths
        /// @param code_lengths pointer to the code lengths
        /// @param code_lengths_num number of code lengths
        /// @param codelen_size number of bits per code length
        /// (supported values: 4 )
        /// ( e.g. 0b00001111 for 4 bits per code length)
        /// @return pointer to generated huffman tree
        tree* gen_tree
        (byte* code_lengths, ushort code_lengths_num, byte codelen_size, byte* next_byte = nullptr) const;
    };
}
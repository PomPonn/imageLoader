#pragma once

// TODO: finish this
namespace deflate {
    typedef unsigned char byte_t;
    typedef unsigned int uword_t;

    struct _data_header {
        byte_t compression;
        /*flags:
        - check bits,
        - preset dictionary flag,
        - compression level
        */
        byte_t flags;
        // if preset dictionary flag is set...
        uword_t preset_dict_id;
    };
    struct data_block_header {

    };
}

/* TEMP INFO
compr_meth = 8; // deflate
compr_info = 7; // value of 7 indicates a 32768 bytes window size
*/
#pragma once

#include <string>

namespace img_loader {
    enum error_type {
        ERR_NULL,
        ERR_OPEN_FAIL,
        ERR_FILE_FORMAT,
        ERR_BMP_RLE,
        ERR_BMP_BITFIELD,
        ERR_FILE_STRUCT,
    };

    void _set_error(error_type err);

    // check whether an error occurred
    bool is_error();

    // get error string (if there is any error)
    std::string get_error_str();
}
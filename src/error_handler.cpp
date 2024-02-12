#include "error_handler.hpp"

namespace img_loader {
    error_type _error = ERR_NULL;

    void _set_error(error_type err) {
        if (err > -1)
            _error = err;
    }

    bool is_error() {
        return _error;
    }

    std::string get_error_str() {
        std::string output = "[IMG_LOADER ERROR]: ";

        switch (_error)
        {
        case ERR_BMP_BITFIELD:
            output += "Loaded bitmap file contains unsupported bit fields.";
            break;
        case ERR_BMP_RLE:
            output += "Loaded bitmap file is compressed with RLE encoding, which isn't implemented.";
            break;
        case ERR_OPEN_FAIL:
            output += "Failed to load a file.";
            break;
        case ERR_FILE_STRUCT:
            output += "Loaded bitmap file has unsupported structure.";
            break;
        case ERR_FILE_FORMAT:
            output += "Loaded file has unsupported format (extension).";
            break;
        default:
            break;
        }
        return output;
    }
}
#include "img_loader.hpp"

#include "bmp.hpp"
#include "png.hpp"

#include <cstring>

namespace img_loader {
    byte* load(const char* file_path, int& width, int& height, pixel_info* px_info)
    {
        FILE* file;
        fopen_s(&file, file_path, "rb");

        if (!file) {
            _set_error(ERR_FILE_FORMAT);
            return nullptr;
        }
        else {
            _extension ext = _get_extension(file_path);

            // check if extension is unknown
            if (!ext) {
                _set_error(ERR_FILE_FORMAT);
                return nullptr;
            }

            switch (ext)
            {
            case _EXT_BMP: {
                return _load_bitmap(file, width, height, px_info);
            }
            case _EXT_PNG: {
                return _load_png_file(file, width, height, px_info);
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
        while (*ptr-- != '.') {}
        ptr++;

        // now ptr contains the extension name

        if (strcmp(ptr, "jpg") || strcmp(ptr, "jpeg") || strcmp(ptr, "jpe") || strcmp(ptr, "jfif")) return _EXT_JPG;
        else if (strcmp(ptr, "png")) return _EXT_PNG;
        else if (strcmp(ptr, "bmp") || strcmp(ptr, "dib")) return _EXT_BMP;
        else return _EXT_UNKNOWN;
    }
};
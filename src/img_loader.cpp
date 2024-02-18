#include "img_loader.hpp"

#include "bmp.hpp"
#include "png.hpp"
#include "jpg.hpp"

#include <string>

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
                return _load_jpeg_file(file, width, height, px_info);
            }
            default: {
                return nullptr;
            }
            }
        }
    }

    void free(byte* data) {
        delete[] data;
    }

    _extension _get_extension(const char* file_path) {
        char* ptr = (char*)file_path;

        while (*ptr++) {}
        while (*ptr != '.') {
            ptr--;
        }
        ptr++;

        // now ptr contains the extension name

        std::string ext(ptr);

        if (ext == "jpg" || ext == "jpeg" || ext == "jpe" || ext == "jfif") return _EXT_JPG;
        else if (ext == "png") return _EXT_PNG;
        else if (ext == "bmp" || ext == "dib") return _EXT_BMP;
        else return _EXT_UNKNOWN;
    }
};
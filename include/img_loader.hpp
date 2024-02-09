#pragma once

#include "common.hpp"

namespace img_loader {
    _extension _get_extension(const char* file_name);

    /// @brief loads file from disk
    /// @param file_path path to the file
    /// @param width width of the file
    /// @param height height of the file
    /// @param format_info optional format information
    /// @return pointer to image data
    byte* load(const char* file_path, int& width, int& height, pixel_info* format_info = nullptr);

    /// @brief frees image data
    /// @param data image data to free
    void free(byte* data);
}
#include "img_loader.hpp"

#include <iostream>

#define FILE_PATH "..."

int main() {

    int width, height;
    img_loader::pixel_info* pinfo = nullptr;
    unsigned char* data = img_loader::load(FILE_PATH, width, height, pinfo);

    if (img_loader::is_error()) {
        std::cerr << img_loader::get_error_str();
    }

    return 0;
}

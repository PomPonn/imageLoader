#include "img_loader.hpp"

#include <Windows.h>
#include <iostream>

#define FILE_PATH "C:/Users/gangs/OneDrive/Dokumenty/Stuff/Projects/Image_loader/temp/sample.bmp"

int main() {

    img_loader::image_data data = img_loader::load(FILE_PATH);

    std::cout << data.isValid();

    return 0;
}
#include "img_loader.hpp"

#include <Windows.h>
#include <iostream>

#define FILE_PATH "C:/Users/gangs/OneDrive/Dokumenty/Stuff/Projects/Image_loader/temp/1.1-MB.bmp"
#define FILE_SIZE 518400

int main() {

    int width, height;
    img_loader::byte* data = img_loader::load(FILE_PATH, width, height);

    return 0;
}
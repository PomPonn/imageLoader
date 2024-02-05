#include "img_loader.hpp"

#include <iostream>

#define FILE_PATH "C:/Users/gangs/OneDrive/Dokumenty/Stuff/Projects/Image_loader/temp/beach.bmp"
#define FILE_SIZE 518400

int main() {

    int width, height;

    img_loader::byte* data = img_loader::load(FILE_PATH, width, height);

    for (int i = 0; i < FILE_SIZE + 1; i++) {
        std::cout << (int)data[i] << ", ";
        if (i % 3 == 0) {
            std::cout << std::endl;
        }
    }

    return 0;
}
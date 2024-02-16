cmake_minimum_required(VERSION 3.8.0)
project(img_loader VERSION 0.1.0 LANGUAGES C CXX)

add_library(img_loader STATIC src/img_loader.cpp)
target_sources(img_loader PRIVATE
src/indian.hpp
src/bmp.cpp
src/png.cpp
src/error_handler.cpp
src/inflate.cpp
src/huffman.cpp)

target_include_directories(img_loader PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_executable(test test_entry.cpp)
target_link_libraries(test ${PROJECT_BINARY_DIR}/debug/img_loader.lib)
target_include_directories(test PUBLIC ${PROJECT_SOURCE_DIR}/include)
cmake_minimum_required(VERSION 3.8.0)
project(img_loader VERSION 0.1.0 LANGUAGES C CXX)

add_library(img_loader STATIC src/img_loader.cpp)

target_include_directories(img_loader PRIVATE ${PROJECT_SOURCE_DIR}/include)

add_executable(test test_entry.cpp)
target_link_libraries(test ${PROJECT_SOURCE_DIR}/build/Debug/img_loader.lib)
target_include_directories(test PUBLIC ${PROJECT_SOURCE_DIR}/include)
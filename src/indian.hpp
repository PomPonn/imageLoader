#pragma once

#include <stdint.h>

/// @brief reverses byte order of specified memory block
/// @param bytes_count
/// @param bytes pointer to memory block
/// @return pointer to new reversed memory
void* reverse_indianness(uint16_t bytes_count, void* bytes) {
    if (bytes_count < 1 || !bytes) return nullptr;

    uint8_t* result = new uint8_t[bytes_count];

    for (uint16_t i = 0; i < bytes_count; i++)
        *(result + i) = *((uint8_t*)bytes + bytes_count - i - 1);

    return (void*)result;
}
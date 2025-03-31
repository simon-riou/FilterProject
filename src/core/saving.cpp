#include "core/saving.h"

#include <fstream>
#include <iostream>
#include <cstdint>

bool saveAsPPM(const sf::Image& image, const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) return false;

    sf::Vector2u size = image.getSize();
    file << "P6\n" << size.x << " " << size.y << "\n255\n";
    const uint8_t* pixels = image.getPixelsPtr();
    for (unsigned y = 0; y < size.y; ++y) {
        for (unsigned x = 0; x < size.x; ++x) {
            std::size_t i = 4 * (y * size.x + x);
            // Ignore alpha
            file.put(pixels[i]);     // R
            file.put(pixels[i + 1]); // G
            file.put(pixels[i + 2]); // B
        }
    }
    return true;
}

bool saveAsTGA(const sf::Image& image, const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) return false;

    sf::Vector2u size = image.getSize();
    const uint8_t* pixels = image.getPixelsPtr();

    // Header TGA (18 octets)
    unsigned char header[18] = {};
    header[2] = 2; // Type d'image : 2 = non compressé, true-color
    header[12] = size.x & 0xFF;
    header[13] = (size.x >> 8) & 0xFF;
    header[14] = size.y & 0xFF;
    header[15] = (size.y >> 8) & 0xFF;
    header[16] = 32; // 32 bits par pixel (RGBA)
    header[17] = 0x20; // Orientation top-left (bit 5 = 1)

    file.write(reinterpret_cast<char*>(header), 18);

    for (unsigned y = 0; y < size.y; ++y) {
        for (unsigned x = 0; x < size.x; ++x) {
            std::size_t i = 4 * (y * size.x + x);
            file.put(pixels[i + 2]); // B
            file.put(pixels[i + 1]); // G
            file.put(pixels[i + 0]); // R
            file.put(pixels[i + 3]); // A
        }
    }

    return true;
}

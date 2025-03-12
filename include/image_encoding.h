#pragma once

#include <SFML/Graphics.hpp>

inline sf::Texture gray_scale(sf::Texture texture) {
    sf::Image image = texture.copyToImage();
    sf::Vector2u size = image.getSize();

    std::vector<std::uint8_t> pixels;
    const std::uint8_t* originalPixels = image.getPixelsPtr();
    if (!originalPixels) {
        std::cerr << "Erreur: impossible de récupérer les pixels." << std::endl;
        return sf::Texture();
    }
    pixels.assign(originalPixels, originalPixels + size.x * size.y * 4);

    for (unsigned int y = 0; y < size.y; ++y) {
        for (unsigned int x = 0; x < size.x; ++x) {
            size_t index = (x + y * size.x) * 4;
            float r = pixels[index];
            float g = pixels[index + 1];
            float b = pixels[index + 2];
            int gray = static_cast<int>(std::round(r * 0.21f + g * 0.72f + b * 0.07f));
            gray = std::max(0, std::min(255, gray));
            pixels[index] = static_cast<std::uint8_t>(gray);
            pixels[index + 1] = static_cast<std::uint8_t>(gray);
            pixels[index + 2] = static_cast<std::uint8_t>(gray);
        }
    }

    sf::Image outputImage(size, pixels.data());

    sf::Texture outputTexture;
    if (!outputTexture.loadFromImage(outputImage)) {
        std::cerr << "Erreur lors du chargement de la texture de sortie." << std::endl;
    }
    return outputTexture;
}

inline sf::Texture combine_channels(sf::Texture texture, bool redSelected, bool greenSelected, bool blueSelected) {
    sf::Image image = texture.copyToImage();
    sf::Vector2u size = image.getSize();

    std::vector<std::uint8_t> pixels;
    const std::uint8_t* originalPixels = image.getPixelsPtr();
    if (!originalPixels) {
        std::cerr << "Erreur: impossible de récupérer les pixels." << std::endl;
        return sf::Texture();
    }
    pixels.assign(originalPixels, originalPixels + size.x * size.y * 4);

    for (unsigned int y = 0; y < size.y; ++y) {
        for (unsigned int x = 0; x < size.x; ++x) {
            size_t index = (x + y * size.x) * 4;
            float r = pixels[index];
            float g = pixels[index + 1];
            float b = pixels[index + 2];
            pixels[index] = static_cast<std::uint8_t>(redSelected ? r : 0);
            pixels[index + 1] = static_cast<std::uint8_t>(greenSelected ? g : 0);
            pixels[index + 2] = static_cast<std::uint8_t>(blueSelected ? b : 0);
        }
    }

    sf::Image outputImage(size, pixels.data());

    sf::Texture outputTexture;
    if (!outputTexture.loadFromImage(outputImage)) {
        std::cerr << "Erreur lors du chargement de la texture de sortie." << std::endl;
    }
    return outputTexture;
}
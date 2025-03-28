#include "core/ImageEncoder.h"

#include <iostream>

sf::Texture ImageEncoder::toGrayscale(const sf::Texture& texture) {
    sf::Image image = texture.copyToImage();
    sf::Vector2u size = image.getSize();

    for (unsigned x = 0; x < size.x; ++x) {
        for (unsigned y = 0; y < size.y; ++y) {
            sf::Color pixel = image.getPixel({x, y});
            uint8_t gray = static_cast<uint8_t>(0.299 * pixel.r + 0.587 * pixel.g + 0.114 * pixel.b);
            image.setPixel({x, y}, sf::Color(gray, gray, gray));
        }
    }

    sf::Texture result;
    bool hasLoaded = result.loadFromImage(image);
    if (!hasLoaded) {
        std::cerr << "Erreur lors du chaegement de la texure depuis l'image." << std::endl;
        exit(1);
    }
    return result;
}

sf::Texture ImageEncoder::combineChannels(const sf::Texture& texture, bool red, bool green, bool blue) {
    sf::Image image = texture.copyToImage();
    sf::Vector2u size = image.getSize();

    for (unsigned x = 0; x < size.x; ++x) {
        for (unsigned y = 0; y < size.y; ++y) {
            sf::Color pixel = image.getPixel({x, y});
            image.setPixel({x, y}, sf::Color(
                red ? pixel.r : 0,
                green ? pixel.g : 0,
                blue ? pixel.b : 0
            ));
        }
    }

    sf::Texture result;
    bool hasLoaded = result.loadFromImage(image);
    if (!hasLoaded) {
        std::cerr << "Erreur lors du chaegement de la texure depuis l'image." << std::endl;
        exit(1);
    }
    return result;
}
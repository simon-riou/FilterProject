#pragma once

#include <SFML/Graphics.hpp>

inline sf::Texture gray_scale(sf::Texture texture) {
    // R�cup�rer l'image � partir de la texture
    sf::Image image = texture.copyToImage();
    sf::Vector2u size = image.getSize();

    // R�cup�rer le tableau de pixels de l'image (taille = width * height * 4)
    std::vector<std::uint8_t> pixels;
    const std::uint8_t* originalPixels = image.getPixelsPtr();
    if (!originalPixels) {
        std::cerr << "Erreur: impossible de r�cup�rer les pixels." << std::endl;
        return sf::Texture();
    }
    pixels.assign(originalPixels, originalPixels + size.x * size.y * 4);

    // Parcourir tous les pixels et appliquer la conversion en niveau de gris
    for (unsigned int y = 0; y < size.y; ++y) {
        for (unsigned int x = 0; x < size.x; ++x) {
            size_t index = (x + y * size.x) * 4;
            float r = pixels[index];
            float g = pixels[index + 1];
            float b = pixels[index + 2];
            // Calcul du niveau de gris selon la pond�ration donn�e
            int gray = static_cast<int>(std::round(r * 0.21f + g * 0.72f + b * 0.07f));
            gray = std::max(0, std::min(255, gray));
            pixels[index] = static_cast<std::uint8_t>(gray);
            pixels[index + 1] = static_cast<std::uint8_t>(gray);
            pixels[index + 2] = static_cast<std::uint8_t>(gray);
            // On laisse l'alpha inchang�
        }
    }

    // Cr�er une nouvelle image avec les pixels modifi�s
    sf::Image outputImage(size, pixels.data());

    // Cr�er une texture et la charger avec la nouvelle image
    sf::Texture outputTexture;
    if (!outputTexture.loadFromImage(outputImage)) {
        std::cerr << "Erreur lors du chargement de la texture de sortie." << std::endl;
    }
    return outputTexture;
}

inline sf::Texture combine_channels(sf::Texture texture, bool redSelected, bool greenSelected, bool blueSelected) {
    // R�cup�rer l'image � partir de la texture
    sf::Image image = texture.copyToImage();
    sf::Vector2u size = image.getSize();

    // R�cup�rer le tableau de pixels de l'image (taille = width * height * 4)
    std::vector<std::uint8_t> pixels;
    const std::uint8_t* originalPixels = image.getPixelsPtr();
    if (!originalPixels) {
        std::cerr << "Erreur: impossible de r�cup�rer les pixels." << std::endl;
        return sf::Texture();
    }
    pixels.assign(originalPixels, originalPixels + size.x * size.y * 4);

    // Parcourir tous les pixels et appliquer la conversion en niveau de gris
    for (unsigned int y = 0; y < size.y; ++y) {
        for (unsigned int x = 0; x < size.x; ++x) {
            size_t index = (x + y * size.x) * 4;
            float r = pixels[index];
            float g = pixels[index + 1];
            float b = pixels[index + 2];
            pixels[index] = static_cast<std::uint8_t>(redSelected ? r : 0);
            pixels[index + 1] = static_cast<std::uint8_t>(greenSelected ? g : 0);
            pixels[index + 2] = static_cast<std::uint8_t>(blueSelected ? b : 0);
            // On laisse l'alpha inchang�
        }
    }

    // Cr�er une nouvelle image avec les pixels modifi�s
    sf::Image outputImage(size, pixels.data());

    // Cr�er une texture et la charger avec la nouvelle image
    sf::Texture outputTexture;
    if (!outputTexture.loadFromImage(outputImage)) {
        std::cerr << "Erreur lors du chargement de la texture de sortie." << std::endl;
    }
    return outputTexture;
}
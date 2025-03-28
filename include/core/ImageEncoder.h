#pragma once

#include <imgui.h>
#include <imgui-SFML.h>
#include <SFML/Graphics.hpp>

class ImageEncoder {
public:
    static sf::Texture toGrayscale(const sf::Texture& texture);
    static sf::Texture combineChannels(const sf::Texture& texture, bool red, bool green, bool blue);
};

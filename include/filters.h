#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <cmath>

inline sf::Texture average_filter(const sf::Texture& texture) {
    return texture;
}

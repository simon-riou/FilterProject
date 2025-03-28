#pragma once
#include <SFML/Graphics/Texture.hpp>
#include <string>

class IFilter {
public:
    virtual ~IFilter() = default;
    virtual std::string name() const = 0;
    virtual sf::Texture apply(const sf::Texture& input) = 0;
};

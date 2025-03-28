#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

#include "filters/types/ConvolutionType.h"

void computeCPUNaiveConvolution(std::vector<std::uint8_t>& pixels, sf::Vector2u size,
    const std::vector<double>& kernel, size_t kernel_size, PaddingType paddingtype);
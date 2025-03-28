#pragma once

#include <cstdint>
#include <vector>
#include <SFML/System/Vector2.hpp>

#include "filters/types/ConvolutionType.h"

void computeGPUNaiveConvolution(std::vector<std::uint8_t>& pixels, sf::Vector2u size, const std::vector<double>& kernel, size_t kernel_size, PaddingType paddingtype);

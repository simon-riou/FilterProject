#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "filters/types/ConvolutionType.h"

std::vector<double> getKernel(ConvolutionType ConvolutionType, size_t kernel_size);

void computeConvolution(std::vector<std::uint8_t>& pixels, sf::Vector2u size,
                      const std::vector<double>& kernel, size_t kernel_size, PaddingType paddingtype);

sf::Texture applyConvolutionFilter(const sf::Texture& texture, ConvolutionType ConvolutionType, size_t kernel_size,
                              const std::vector<double>& custom_kernel = {},
                              PaddingType paddingtype = PaddingType::ZERO);

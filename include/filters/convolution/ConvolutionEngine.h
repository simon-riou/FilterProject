#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "filters/types/ConvolutionType.h"
#include "filters/types/BackendType.h"
#include "filters/types/AlgorithmType.h"

std::vector<double> getKernel(ConvolutionType ConvolutionType, size_t kernel_size);

sf::Texture applyConvolutionFilter(const sf::Texture& texture, ConvolutionType ConvolutionType, size_t kernel_size,
                              const std::vector<double>& custom_kernel,
                              PaddingType paddingtype, BackendType Backendtype, AlgorithmType Algorithmtype);

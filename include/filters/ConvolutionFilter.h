#pragma once

#include "filters/IFilter.h"
#include "filters/types/ConvolutionType.h"

#include <vector>

class ConvolutionFilter : public IFilter {
public:
    ConvolutionFilter(ConvolutionType type, int kernelSize, const std::vector<double>& customKernel = {}, PaddingType padding = PaddingType::REPLICATE);

    std::string name() const override;
    sf::Texture apply(const sf::Texture& input) override;

private:
    ConvolutionType type;
    int kernelSize;
    std::vector<double> customKernel;
    PaddingType padding;
};

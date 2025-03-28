#pragma once

#include "filters/IFilter.h"
#include "filters/types/ConvolutionType.h"
#include "filters/types/BackendType.h"
#include "filters/types/AlgorithmType.h"

#include <vector>

class ConvolutionFilter : public IFilter {
public:
    ConvolutionFilter(ConvolutionType type, int kernelSize, PaddingType padding, BackendType backend, AlgorithmType algorithm, std::vector<double>& customKernel);

    std::string name() const override;
    sf::Texture apply(const sf::Texture& input) override;

private:
    ConvolutionType type;
    PaddingType padding;
    BackendType backend;
    AlgorithmType algorithm;

    int kernelSize;
    std::vector<double> customKernel;
};

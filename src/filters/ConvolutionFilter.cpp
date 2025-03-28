#include "filters/ConvolutionFilter.h"
#include "filters/convolution/ConvolutionEngine.h"

ConvolutionFilter::ConvolutionFilter(ConvolutionType type, int kernelSize, const std::vector<double>& customKernel, PaddingType padding)
    : type(type), kernelSize(kernelSize), customKernel(customKernel), padding(padding) {}

std::string ConvolutionFilter::name() const {
    return "Convolution";
}

sf::Texture ConvolutionFilter::apply(const sf::Texture& input) {
    return applyConvolutionFilter(input, type, kernelSize, customKernel, padding);
}
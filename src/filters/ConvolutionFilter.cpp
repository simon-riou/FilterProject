#include "filters/ConvolutionFilter.h"
#include "filters/convolution/ConvolutionEngine.h"

ConvolutionFilter::ConvolutionFilter(ConvolutionType type, int kernelSize, double sigma, PaddingType padding, BackendType backend, AlgorithmType algorithm, std::vector<double>& customKernel)
    : type(type), kernelSize(kernelSize), sigma(sigma), padding(padding), backend(backend), algorithm(algorithm), customKernel(customKernel) {}

std::string ConvolutionFilter::name() const {
    return "Convolution";
}

sf::Texture ConvolutionFilter::apply(const sf::Texture& input) {
    return applyConvolutionFilter(input, type, kernelSize, sigma, customKernel, padding, backend, algorithm);
}
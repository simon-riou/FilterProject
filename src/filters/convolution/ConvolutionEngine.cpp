#include "filters/convolution/ConvolutionCpu.h"
#include "filters/convolution/ConvolutionGpu.cuh"

#include "filters/convolution/ConvolutionCpu.h"
#include "filters/convolution/ConvolutionEngine.h"
#include "filters/convolution/KernelComputer.h"

#include <iostream>
#include <algorithm>
#include <cmath>

std::vector<double> getKernel(ConvolutionType ConvolutionType, size_t kernel_size) {
    switch (ConvolutionType) {
        case ConvolutionType::IDENTITY: return compute_identity_kernel();
        case ConvolutionType::MEAN: return compute_mean_kernel(kernel_size);
        case ConvolutionType::GAUSSIAN: return compute_gaussian_kernel(kernel_size);
        case ConvolutionType::SHARPEN: return compute_sharpen_kernel();
        case ConvolutionType::LAPLACIEN: return compute_laplacien_kernel();
        case ConvolutionType::EDGE_REINFORCEMENT_HOR: return compute_edge_reiforcement_hor();
        case ConvolutionType::EDGE_REINFORCEMENT_VER: return compute_edge_reiforcement_ver();
        default: return {};
    }
}

sf::Texture applyConvolutionFilter(const sf::Texture& texture, ConvolutionType ConvolutionType, size_t kernel_size,
                              const std::vector<double>& custom_kernel, PaddingType paddingtype, BackendType backend, AlgorithmType algorithm) {
    if (ConvolutionType == ConvolutionType::CUSTOM && custom_kernel.empty()) {
        throw std::invalid_argument("The custom kernel must be provided.");
    }
    std::vector<double> kernel = getKernel(ConvolutionType, kernel_size);

    sf::Image image = texture.copyToImage();
    sf::Vector2u size = image.getSize();

    std::vector<std::uint8_t> pixels;
    const std::uint8_t* originalPixels = image.getPixelsPtr();
    if (!originalPixels) {
        std::cerr << "Erreur: impossible de r�cup�rer les pixels." << std::endl;
        return sf::Texture();
    }
    pixels.assign(originalPixels, originalPixels + size.x * size.y * 4);

    switch (ConvolutionType) {
        case ConvolutionType::IDENTITY:
            return texture;
        case ConvolutionType::MEAN:
        case ConvolutionType::GAUSSIAN:
            if (backend == BackendType::CPU && algorithm == AlgorithmType::Naive)
                computeCPUNaiveConvolution(pixels, size, kernel, kernel_size, paddingtype);
            if (backend == BackendType::GPU_CUSTOM && algorithm == AlgorithmType::Naive) {
                computeGPUNaiveConvolution(pixels, size, kernel, kernel_size, paddingtype);
            }
            break;
        case ConvolutionType::SHARPEN:
        case ConvolutionType::LAPLACIEN:
        case ConvolutionType::EDGE_REINFORCEMENT_HOR:
        case ConvolutionType::EDGE_REINFORCEMENT_VER:
            if (backend == BackendType::CPU && algorithm == AlgorithmType::Naive)
                computeCPUNaiveConvolution(pixels, size, kernel, kernel_size, paddingtype);
            if (backend == BackendType::GPU_CUSTOM && algorithm == AlgorithmType::Naive)
                computeGPUNaiveConvolution(pixels, size, kernel, kernel_size, paddingtype);
            break;
        case ConvolutionType::CUSTOM:
            if (backend == BackendType::CPU && algorithm == AlgorithmType::Naive)
                computeCPUNaiveConvolution(pixels, size, custom_kernel, kernel_size, paddingtype);
            if (backend == BackendType::GPU_CUSTOM && algorithm == AlgorithmType::Naive)
                computeGPUNaiveConvolution(pixels, size, kernel, kernel_size, paddingtype);
            break;
    }

    sf::Image outputImage(size, pixels.data());

    sf::Texture outputTexture;
    if (!outputTexture.loadFromImage(outputImage)) {
        std::cerr << "Erreur lors du chargement de la texture de sortie." << std::endl;
    }
    return outputTexture;
}

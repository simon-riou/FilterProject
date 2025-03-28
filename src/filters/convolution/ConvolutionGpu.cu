#include "filters/convolution/ConvolutionGpu.cuh"
#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <iostream>
#include <stdexcept>

__device__ uint8_t get_channel(const uint8_t* input, int x, int y, int channel, int width, int height, int pad, PaddingType paddingtype) {
    if (x < 0 || y < 0 || x >= width || y >= height) {
        if (paddingtype == PaddingType::ZERO)
            return 0;
        if (paddingtype == PaddingType::REPLICATE) {
            x = max(0, min(x, width - 1));
            y = max(0, min(y, height - 1));
        }
    }
    return input[(y * width + x) * 4 + channel];
}

__global__ void convolutionKernel(uint8_t* input, uint8_t* output, double* kernel, int width, int height, int kernelSize, PaddingType paddingtype) {
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    if (x >= width || y >= height) return;

    int pad = kernelSize / 2;
    double sum_r = 0.0, sum_g = 0.0, sum_b = 0.0;

    for (int ky = -pad; ky <= pad; ++ky) {
        for (int kx = -pad; kx <= pad; ++kx) {
            double k_val = kernel[(ky + pad) * kernelSize + (kx + pad)];
            sum_r += get_channel(input, x + kx, y + ky, 0, width, height, pad, paddingtype) * k_val;
            sum_g += get_channel(input, x + kx, y + ky, 1, width, height, pad, paddingtype) * k_val;
            sum_b += get_channel(input, x + kx, y + ky, 2, width, height, pad, paddingtype) * k_val;
        }
    }

    int idx = (y * width + x) * 4;
    output[idx]     = min(max(int(sum_r), 0), 255);
    output[idx + 1] = min(max(int(sum_g), 0), 255);
    output[idx + 2] = min(max(int(sum_b), 0), 255);
    output[idx + 3] = 255; // Alpha inchangé
}

void computeGPUNaiveConvolution(std::vector<std::uint8_t>& pixels, sf::Vector2u size, const std::vector<double>& kernel, size_t kernel_size, PaddingType paddingtype) {
    if (kernel_size % 2 == 0 || kernel.size() != kernel_size * kernel_size) {
        throw std::invalid_argument("The kernel must be odd and squared.");
    }

    int imgSize = size.x * size.y * 4;
    uint8_t* d_input = nullptr;
    uint8_t* d_output = nullptr;
    double* d_kernel = nullptr;

    cudaMalloc(&d_input, imgSize);
    cudaMalloc(&d_output, imgSize);
    cudaMalloc(&d_kernel, kernel.size() * sizeof(double));

    cudaMemcpy(d_input, pixels.data(), imgSize, cudaMemcpyHostToDevice);
    cudaMemcpy(d_kernel, kernel.data(), kernel.size() * sizeof(double), cudaMemcpyHostToDevice);

    dim3 threads(16, 16);
    dim3 blocks((size.x + threads.x - 1) / threads.x, (size.y + threads.y - 1) / threads.y);
    convolutionKernel<<<blocks, threads>>>(d_input, d_output, d_kernel, size.x, size.y, kernel_size, paddingtype);
    cudaDeviceSynchronize();

    cudaMemcpy(pixels.data(), d_output, imgSize, cudaMemcpyDeviceToHost);

    cudaFree(d_input);
    cudaFree(d_output);
    cudaFree(d_kernel);
}

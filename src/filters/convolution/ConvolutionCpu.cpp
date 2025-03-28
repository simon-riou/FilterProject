#include "filters/convolution/ConvolutionCpu.h"

#include <algorithm>
#include <iostream>

void computeCPUNaiveConvolution(std::vector<std::uint8_t>& pixels, sf::Vector2u size, const std::vector<double>& kernel, size_t kernel_size, PaddingType paddingtype) {
    if (kernel_size % 2 == 0 || kernel.size() != kernel_size * kernel_size) {
        throw std::invalid_argument("The kernel must be odd and squared.");
    }

    int pad = kernel_size / 2;
    std::vector<std::uint8_t> output(pixels.size(), 255);

    auto get_pixel = [&](int x, int y) -> std::vector<uint8_t> {
        if (x < 0 || y < 0 || x >= static_cast<int>(size.x) || y >= static_cast<int>(size.y)) {
            if (paddingtype == PaddingType::ZERO) return { 0, 0, 0 };
            if (paddingtype == PaddingType::REPLICATE) {
                x = std::clamp(x, 0, static_cast<int>(size.x) - 1);
                y = std::clamp(y, 0, static_cast<int>(size.y) - 1);
            }
        }
        return { pixels[(y * size.x + x) * 4], pixels[(y * size.x + x) * 4 + 1], pixels[(y * size.x + x) * 4 + 2] };
        };

    for (size_t y = 0; y < size.y; ++y) {
        std::clog << "\rScanlines remaining: " << ((100 * y) / size.y) << " %" << std::flush;
        for (size_t x = 0; x < size.x; ++x) {
            double sum_r = 0.0f;
            double sum_g = 0.0f;
            double sum_b = 0.0f;

            for (int ky = -pad; ky <= pad; ++ky) {
                for (int kx = -pad; kx <= pad; ++kx) {
                    std::vector<uint8_t> pixels = get_pixel(x + kx, y + ky);
                    double kernel_value = kernel[(ky + pad) * kernel_size + (kx + pad)];
                    sum_r += pixels[0] * kernel_value;
                    sum_g += pixels[1] * kernel_value;
                    sum_b += pixels[2] * kernel_value;
                }
            }

            output[(y * size.x + x) * 4]     = static_cast<std::uint8_t>(std::clamp(static_cast<int>(sum_r), 0, 255));
            output[(y * size.x + x) * 4 + 1] = static_cast<std::uint8_t>(std::clamp(static_cast<int>(sum_g), 0, 255));
            output[(y * size.x + x) * 4 + 2] = static_cast<std::uint8_t>(std::clamp(static_cast<int>(sum_b), 0, 255));
        }
    }

    std::clog << "\rDone.                                   \n";

    pixels = output;
}
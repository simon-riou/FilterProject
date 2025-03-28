#include "filters/convolution/KernelComputer.h"

#include <cmath>

std::vector<double> compute_identity_kernel() {
	return { 1 };
}

std::vector<double> compute_mean_kernel(size_t kernel_size) {
	return std::vector<double>(kernel_size * kernel_size, 1.0f / static_cast<double>(kernel_size * kernel_size));
}

std::vector<double> compute_gaussian_kernel(size_t kernel_size) {
	int n = kernel_size;
    int m = kernel_size;

	double sigma = 1.0;

    std::vector<double> h(m * n, 0.0);
    double sum = 0.0;

    int half_m = (m - 1) / 2;
    int half_n = (n - 1) / 2;

    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            double x = j - half_n;
            double y = i - half_m;
            h[i * m + j] = std::exp(-(x * x + y * y) / (2 * sigma * sigma));
            sum += h[i * m + j];
        }
    }

    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            h[i * m + j] /= sum;
        }
    }

    return h;
}

std::vector<double> compute_sharpen_kernel() {
	return { 0, -1, 0, -1, 5, -1, 0, -1, 0 };
}

std::vector<double> compute_laplacien_kernel() {
	return { 0, 1, 0, 1, -4, 1, 0, 1, 0 };
}

std::vector<double> compute_edge_reiforcement_hor() {
	return { 0, 0, 0, -1, 2, -1, 0, 0, 0 };
}

std::vector<double> compute_edge_reiforcement_ver() {
	return { 0, -1, 0, 0, 2, 0, 0, -1, 0 };
}
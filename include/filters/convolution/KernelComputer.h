#pragma once

#include <vector>
#include <cstddef>

std::vector<double> compute_identity_kernel();
std::vector<double> compute_mean_kernel(size_t size);
std::vector<double> compute_gaussian_kernel(size_t size);
std::vector<double> compute_sharpen_kernel();
std::vector<double> compute_laplacien_kernel();
std::vector<double> compute_edge_reiforcement_hor();
std::vector<double> compute_edge_reiforcement_ver();
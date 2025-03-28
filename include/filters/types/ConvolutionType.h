#pragma once

enum class ConvolutionType {
    CUSTOM,
    IDENTITY,
    MEAN,
    GAUSSIAN,
    SHARPEN,
    LAPLACIEN,
    EDGE_REINFORCEMENT_HOR,
    EDGE_REINFORCEMENT_VER,
};

enum class PaddingType {
    ZERO,
    REPLICATE
};

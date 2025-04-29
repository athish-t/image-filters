# pragma once

#include "image_filter.hpp"
#include "types.hpp"

#include <opencv2/opencv.hpp>

class SobelOperator : public ImageFilter
{
public:
    static constexpr int KERNELX[3][3] = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };

    static constexpr int KERNELY[3][3] = {
        {1, 2, 1},
        {0, 0, 0},
        {-1, -2, -1}
    };

    SobelOperator() {}

    void apply(const FlatImage& input, FlatImage& output) const override;
    void applyBenchmark(const cv::Mat& input, cv::Mat& output) const override;
};

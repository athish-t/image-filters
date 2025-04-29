# pragma once

#include "image_filter.hpp"
#include "types.hpp"

#include <opencv2/opencv.hpp>

class Blur : public ImageFilter
{
public:
    static constexpr float KERNEL[3][3] = {
        {.11, .11, .11},
        {.11, .11, .11},
        {.11, .11, .11}
    };

    Blur() {}

    void apply(const FlatImage& input, FlatImage& output) const override;
    void applyBenchmark(const cv::Mat& input, cv::Mat& output) const override;
};

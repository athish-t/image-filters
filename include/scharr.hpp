# pragma once

#include "image_filter.hpp"
#include "types.hpp"

#include <opencv2/opencv.hpp>

class ScharrOperator : public ImageFilter
{
public:
    static constexpr int KERNELX[3][3] = {
        {-3, 0, 3},
        {-10, 0, 10},
        {-3, 0, 3}
    };

    static constexpr int KERNELY[3][3] = {
        {3, 10, 3},
        {0, 0, 0},
        {-3, -10, -3}
    };

    ScharrOperator() {}

    void apply(const FlatImage& input, FlatImage& output) const override;
    void applyBenchmark(const cv::Mat& input, cv::Mat& output) const override;

private:
    void applyKernel(const FlatImage& input, FlatImage& output, int rows, int cols);
};

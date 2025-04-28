# pragma once

#include "IOperator.hpp"
#include <opencv2/opencv.hpp>

class SobelOperator : public IOperator
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

    SobelOperator(const cv::Mat& inputImage)
        : _inputImage(inputImage) {}

    void apply(cv::Mat& output) override;
    void applyWithBenchmark(cv::Mat& output) override;

private:
    void applyKernel(const std::vector<uchar>& input, std::vector<uchar>& output, int rows, int cols);

    const cv::Mat& _inputImage;
};

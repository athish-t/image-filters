# pragma once

#include "IOperator.hpp"
#include <opencv2/opencv.hpp>

class ScharrOperator : public IOperator
{
public:
    ScharrOperator(const cv::Mat& inputImage)
        : _inputImage(inputImage) {}

    void apply(cv::Mat& output) override;
    void applyWithBenchmark(cv::Mat& output) override;

private:
    const cv::Mat& _inputImage;
};

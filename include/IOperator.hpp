# pragma once

#include <opencv2/opencv.hpp>

class IOperator
{
public:
    virtual void apply(cv::Mat& output) = 0;
    virtual void applyWithBenchmark(cv::Mat& output) = 0;

    virtual ~IOperator() = default;

};

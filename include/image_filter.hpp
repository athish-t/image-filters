# pragma once

#include <opencv2/opencv.hpp>
#include "types.hpp"

class ImageFilter
{
public:
    virtual void apply(const FlatImage& input, FlatImage& output) const = 0;
    virtual void applyBenchmark(const cv::Mat& input, cv::Mat& output) const = 0;

    virtual ~ImageFilter() = default;

private:


};

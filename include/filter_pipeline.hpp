# pragma once

#include <opencv2/opencv.hpp>
#include "image_filter.hpp"

class FilterPipeline
{
public:
    FilterPipeline& add(const std::shared_ptr<const ImageFilter>& filter);
    FilterPipeline& addBlur();
    FilterPipeline& addScharrOperator();
    FilterPipeline& addSobelOperator();

    void apply(const FlatImage& input, FlatImage& output);
    void applyBenchmark(const cv::Mat& input, cv::Mat& output);

private:
    std::vector<std::shared_ptr<const ImageFilter>> filters;

};

#include "filter_pipeline.hpp"
#include "blur.hpp"
#include "scharr.hpp"
#include "sobel.hpp"

FilterPipeline& FilterPipeline::add(const std::shared_ptr<const ImageFilter>& filter)
{
    filters.push_back(filter);
    return *this;
}

FilterPipeline& FilterPipeline::addBlur()
{
    filters.push_back(std::make_shared<Blur>());
    return *this;
}

FilterPipeline& FilterPipeline::addScharrOperator()
{
    filters.push_back(std::make_shared<ScharrOperator>());
    return *this;
}

FilterPipeline& FilterPipeline::addSobelOperator()
{
    filters.push_back(std::make_shared<SobelOperator>());
    return *this;
}

void FilterPipeline::apply(const FlatImage& input, FlatImage& output)
{
    FlatImage temp = input;
    for (const auto& filter : filters) {
        filter->apply(temp, output);
        temp = std::move(output);
    }
    output = std::move(temp);
}

void FilterPipeline::applyBenchmark(const cv::Mat& input, cv::Mat& output)
{
    cv::Mat temp = input;
    for (const auto& filter : filters) {
        filter->applyBenchmark(temp, output);
        temp = std::move(output);
    }
    output = std::move(temp);
}

#include "scharr.hpp"

void ScharrOperator::applyBenchmark(const cv::Mat& input, cv::Mat& output) const {
    cv::Scharr(input, output, CV_8U, 1, 0);
}

void ScharrOperator::apply(const FlatImage& input, FlatImage& output) const {

}

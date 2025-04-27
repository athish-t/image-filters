#include "scharr.hpp"

void ScharrOperator::applyWithBenchmark(cv::Mat& output) {
    cv::Scharr(_inputImage, output, CV_8U, 1, 0);
}

void ScharrOperator::apply(cv::Mat& output) {
    cv::Scharr(_inputImage, output, CV_8U, 1, 0);
}

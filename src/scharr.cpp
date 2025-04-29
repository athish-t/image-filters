#include "prof_utils.hpp"
#include "scharr.hpp"

void ScharrOperator::applyBenchmark(const cv::Mat& input, cv::Mat& output) const {
    PROF_EXEC_TIME;
    cv::Scharr(input, output, CV_8U, 1, 0);
}

void ScharrOperator::apply(const FlatImage& input, FlatImage& output) const {
    PROF_EXEC_TIME;
    applyXYKernels(input, output, KERNELX, KERNELY, 50);
}

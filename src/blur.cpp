#include <execution>

#include "prof_utils.hpp"
#include "blur.hpp"


void Blur::applyBenchmark(const cv::Mat& input, cv::Mat& output) const {
    PROF_EXEC_TIME;

    cv::blur(input, output, cv::Size(3, 3));
}

void Blur::apply(const FlatImage& input, FlatImage& output) const {
    PROF_EXEC_TIME;
    applySingleKernel(input, output, KERNEL);
}

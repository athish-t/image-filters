#include <execution>

#include "prof_utils.hpp"
#include "sobel.hpp"


void SobelOperator::applyBenchmark(const cv::Mat& input, cv::Mat& output) const {
    PROF_EXEC_TIME;

    cv::Sobel(input, output, CV_8U, 1, 1);
}

void SobelOperator::apply(const FlatImage& input, FlatImage& output) const {
    PROF_EXEC_TIME;
    applyXYkernels(input, output, KERNELX, KERNELY);
}

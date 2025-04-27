#include "sobel.hpp"

void SobelOperator::applyWithBenchmark(cv::Mat& output) {
    cv::Sobel(_inputImage, output, CV_8U, 1, 1);
}

void SobelOperator::apply(cv::Mat& output) {
    cv::Sobel(_inputImage, output, CV_8U, 1, 1);
}

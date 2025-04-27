#ifndef CV_UTILS_HPP
#define CV_UTILS_HPP

#include <opencv2/opencv.hpp>
#include <string>

void displayImages(const cv::Mat& inputImage, const cv::Mat& benchmarkImage, const cv::Mat& customImage);
void writeImage(const std::string& outputPath, const cv::Mat& image);

#endif // CV_UTILS_HPP

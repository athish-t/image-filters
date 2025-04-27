#ifndef CV_UTILS_HPP
#define CV_UTILS_HPP

#include <opencv2/opencv.hpp>
#include <string>

void displayImages(const std::vector<cv::Mat>& images);
void writeImage(const std::string& outputPath, const cv::Mat& image);

#endif // CV_UTILS_HPP

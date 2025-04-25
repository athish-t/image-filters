#include <iostream>
#include <opencv2/opencv.hpp>

int main() {
    std::cout << "Hello, OpenCV with Ninja!" << std::endl;

    // Example OpenCV code
    cv::Mat image = cv::Mat::zeros(100, 100, CV_8UC3);
    if (image.empty()) {
        std::cerr << "Failed to create an image." << std::endl;
        return -1;
    }

    std::cout << "Image created successfully." << std::endl;
    return 0;
}

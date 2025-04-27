#include "cv_utils.hpp"
#include <iostream>

void displayImages(const cv::Mat& inputImage, const cv::Mat& benchmarkImage, const cv::Mat& customImage) {
    // Check if any of the input images are empty
    if (inputImage.empty() || benchmarkImage.empty() || customImage.empty()) {
        std::cerr << "Error: One or more input images are empty." << std::endl;
        return;
    }

    // Ensure all images have the same size and type
    cv::Mat resizedInput, resizedBenchmark, resizedCustom;
    cv::Size targetSize = inputImage.size();

    cv::resize(inputImage, resizedInput, targetSize);
    cv::resize(benchmarkImage, resizedBenchmark, targetSize);
    cv::resize(customImage, resizedCustom, targetSize);

    if (resizedInput.type() != resizedBenchmark.type() || resizedInput.type() != resizedCustom.type()) {
        resizedBenchmark.convertTo(resizedBenchmark, resizedInput.type());
        resizedCustom.convertTo(resizedCustom, resizedInput.type());
    }

    // Combine input, benchmark, and custom images side by side
    cv::Mat combinedImage;
    cv::hconcat(std::vector<cv::Mat>{resizedInput, resizedBenchmark, resizedCustom}, combinedImage);
    cv::imshow("Input, Benchmark, and Custom Images", combinedImage);
    cv::waitKey(0); // Wait for a key press
}

void writeImage(const std::string& outputPath, const cv::Mat& image) {
    if (!cv::imwrite(outputPath, image)) {
        std::cerr << "Failed to write the image to: " << outputPath << std::endl;
    } else {
        std::cout << "Image saved successfully to: " << outputPath << std::endl;
    }
}

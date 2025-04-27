#include "cv_utils.hpp"
#include <iostream>

void displayImages(const std::vector<cv::Mat>& images) {
    if (images.empty()) {
        std::cerr << "Error: No images provided for display." << std::endl;
        return;
    }

    // Check if any image is empty
    for (const auto& img : images) {
        if (img.empty()) {
            std::cerr << "Error: One or more input images are empty." << std::endl;
            return;
        }
    }

    // Resize all images to the size of the first image
    cv::Size targetSize = images[0].size();
    int targetType = images[0].type();
    std::vector<cv::Mat> resizedImages;

    for (const auto& img : images) {
        cv::Mat resized;
        cv::resize(img, resized, targetSize);
        if (resized.type() != targetType) {
            resized.convertTo(resized, targetType);
        }
        resizedImages.push_back(resized);
    }

    // Combine all images side by side
    cv::Mat combinedImage;
    cv::hconcat(resizedImages, combinedImage);
    cv::imshow("Images", combinedImage);
    cv::waitKey(0); // Wait for a key press
}

void writeImage(const std::string& outputPath, const cv::Mat& image) {
    if (!cv::imwrite(outputPath, image)) {
        std::cerr << "Failed to write the image to: " << outputPath << std::endl;
    } else {
        std::cout << "Image saved successfully to: " << outputPath << std::endl;
    }
}

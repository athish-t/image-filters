#include <iostream>

#include "io_utils.hpp"

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

    // Determine grid size (rows and columns)
    int gridCols = static_cast<int>(std::ceil(std::sqrt(images.size())));
    int gridRows = static_cast<int>(std::ceil(static_cast<double>(images.size()) / gridCols));

    // Determine the maximum width and height of images
    int maxWidth = 0, maxHeight = 0;
    for (const auto& img : images) {
        maxWidth = std::max(maxWidth, img.cols);
        maxHeight = std::max(maxHeight, img.rows);
    }

    // Create a blank canvas for the grid with 3 channels (RGB)
    cv::Mat canvas = cv::Mat::zeros(gridRows * maxHeight, gridCols * maxWidth, CV_8UC3);

    // Place each image in the grid
    for (size_t i = 0; i < images.size(); ++i) {
        int row = i / gridCols;
        int col = i % gridCols;

        // Convert the image to 3 channels (RGB) if necessary
        cv::Mat img;
        if (images[i].channels() == 1) {
            cv::cvtColor(images[i], img, cv::COLOR_GRAY2BGR);
        } else {
            img = images[i];
        }

        // Resize the image to fit within the grid cell
        cv::Mat resized;
        cv::resize(img, resized, cv::Size(maxWidth, maxHeight));

        // Copy the resized image to the canvas
        cv::Rect roi(col * maxWidth, row * maxHeight, maxWidth, maxHeight);
        resized.copyTo(canvas(roi));
    }

    // Display the canvas
    cv::imshow("Images", canvas);
    cv::waitKey(0); // Wait for a key press
}

void writeImage(const std::string& outputPath, const cv::Mat& image) {
    if (!cv::imwrite(outputPath, image)) {
        std::cerr << "Failed to write the image to: " << outputPath << std::endl;
    } else {
        std::cout << "Image saved successfully to: " << outputPath << std::endl;
    }
}

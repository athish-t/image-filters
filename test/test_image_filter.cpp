#include <opencv2/opencv.hpp>
#include <gtest/gtest.h>

#include "image_filter.hpp"
#include "test_utils.hpp"

class ImageFilterTest : public ImageFilter , public ::testing::Test {
public:
    void apply(const FlatImage& input, FlatImage& output) const override {}
    void applyBenchmark(const cv::Mat& input, cv::Mat& output) const override {}
};


TEST_F(ImageFilterTest, PadBoundaries) {
    FlatImage input = createTestImage();
    FlatImage output;

    auto [padded_rows, padded_cols] = padBoundaries(input, output);

    ASSERT_EQ(padded_rows, input.rows() + 2);
    ASSERT_EQ(padded_cols, input.cols() + 2);

    ASSERT_EQ(output(0, 0), input(0, 0)); // Top-left corner
    ASSERT_EQ(output(output.rows() - 1, output.cols() - 1), input(input.rows() - 1, input.cols() - 1)); // Bottom-right corner
    ASSERT_EQ(output(0, output.cols() - 1), input(0, input.cols() - 1)); // Top-right corner
    ASSERT_EQ(output(output.rows() - 1, 0), input(input.rows() - 1, 0)); // Bottom-left corner

    for (int i = 1; i < output.rows() - 1; ++i) {
        ASSERT_EQ(output(i, 0), input(i - 1, 0)); // Left boundary
        ASSERT_EQ(output(i, output.cols() - 1), input(i - 1, input.cols() - 1)); // Right boundary
    }

    for (int j = 1; j < output.cols() - 1; ++j) {
        ASSERT_EQ(output(0, j), input(0, j - 1)); // Top boundary
        ASSERT_EQ(output(output.rows() - 1, j), input(input.rows() - 1, j - 1)); // Bottom boundary
    }
}

TEST_F(ImageFilterTest, RemoveBoundaries) {
    FlatImage input = createTestImageWithPadding(); // Helper function to create a padded test image
    FlatImage output;

    removeBoundaries(input, output);

    ASSERT_EQ(output.rows(), input.rows() - 2);
    ASSERT_EQ(output.cols(), input.cols() - 2);
    // Add more assertions to validate the boundary removal
}

TEST_F(ImageFilterTest, GetGradient) {
    FlatImage input = createTestImage();
    FlatImage output;

    const int kernel[3][3] = {
        {0, 1, 0},
        {1, 1, 1},
        {0, 1, 0}
    };

    getGradient(input, output, input.rows(), input.cols(), kernel);

    ASSERT_FALSE(output.empty());
    // Add more assertions to validate the gradient
}

TEST_F(ImageFilterTest, CombineGradients) {
    FlatImage gx = createTestImage();
    FlatImage gy = createTestImage();
    FlatImage combinedGradient;

    combineGradients(gx, gy, combinedGradient, gx.rows(), gx.cols());

    ASSERT_FALSE(combinedGradient.empty());
    // Add more assertions to validate the combined gradient
}

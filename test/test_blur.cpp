#include <opencv2/opencv.hpp>
#include <gtest/gtest.h>

#include "blur.hpp"
#include "test_utils.hpp"


TEST(Blur, TestBlur) {
    FlatImage input = createTestImage(5, 5, 200);
    FlatImage output;

    Blur().apply(input, output);

    ASSERT_FALSE(output.empty());
    ASSERT_EQ(output.rows(), input.rows());
    ASSERT_EQ(output.cols(), input.cols());

    std::vector<std::vector<int>> expectedData = {
        {200,  200,  200,  200, 200},
        {200,  200,  200,  200, 200},
        {200,  200,  200,  200, 200},
        {200,  200,  200,  200, 200},
        {200,  200,  200,  200, 200}
    };
    FlatImage expectedOutput = FlatImageFactory::from(expectedData);

    for (int i = 0; i < output.rows(); ++i) {
        for (int j = 0; j < output.cols(); ++j) {
            ASSERT_NEAR(output(i, j), expectedOutput(i, j), 2);
        }
    }
}

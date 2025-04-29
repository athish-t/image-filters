#include <opencv2/opencv.hpp>
#include <gtest/gtest.h>

#include "scharr.hpp"
#include "test_utils.hpp"


TEST(Scharr, TestSCharr) {
    FlatImage input = createTestImage(5, 5, 1);
    FlatImage output;

    ScharrOperator().apply(input, output);

    ASSERT_FALSE(output.empty());
    ASSERT_EQ(output.rows(), input.rows());
    ASSERT_EQ(output.cols(), input.cols());

    std::vector<std::vector<int>> expectedData = {
        {0,  0,  0,  0, 0},
        {0,  0,  0,  0, 0},
        {0,  0,  0,  0, 0},
        {0,  0,  0,  0, 0},
        {0,  0,  0,  0, 0}
    };
    FlatImage expectedOutput = FlatImageFactory::from(expectedData);

    for (int i = 0; i < output.rows(); ++i) {
        for (int j = 0; j < output.cols(); ++j) {
            ASSERT_EQ(output(i, j), expectedOutput(i, j));
        }
    }
}

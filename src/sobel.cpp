#include <execution>

#include "prof_utils.hpp"
#include "img_utils.hpp"
#include "sobel.hpp"

constexpr int NORMALIZED_GRADIENT_THRESHOLD = 50;
constexpr float NORMALIZATION_FACTOR = 0.5;

void SobelOperator::applyBenchmark(const cv::Mat& input, cv::Mat& output) const {
    PROF_EXEC_TIME;

    cv::Sobel(input, output, CV_8U, 1, 1);
}

void padBoundaries(const FlatImage& input, FlatImage& output, int& padded_rows, int& padded_cols) {
    PROF_EXEC_TIME;

    const int rows = input.rows();
    const int cols = input.cols();

    padded_rows = rows + 2;
    padded_cols = cols + 2;

    output.resize(padded_rows, padded_cols);

    // 1. Copy the inner content in one go using memcpy for each row
    for (int i = 0; i < rows; ++i) {
        std::memcpy(&output(i + 1, 1), &input(i, 0), cols * sizeof(uchar));
    }

    // 2. Handle top and bottom edge rows (without corners)
    std::memcpy(&output(0, 1), &input(0, 0), cols * sizeof(uchar));
    std::memcpy(&output(padded_rows - 1, 1), &input(rows - 1, 0), cols * sizeof(uchar));

    // 3. Handle left and right edge columns
    for (int i = 0; i < rows; ++i) {
        // Left edge
        output(i + 1, 0) = input(i, 0);
        // Right edge
        output(i + 1, padded_cols - 1) = input(i, cols - 1);
    }

    // 4. Handle the four corners
    output(0, 0) = input(0, 0);                             // Top-left
    output(0, padded_cols - 1) = input(0, cols - 1);        // Top-right
    output(padded_rows - 1, 0) = input(rows - 1, 0);        // Bottom-left
    output(padded_rows - 1, padded_cols - 1) = input(rows - 1, cols - 1); // Bottom-right
}


void removeBoundaries(const FlatImage& input, FlatImage& output) {
    PROF_EXEC_TIME;

    const int padded_rows = input.rows();
    const int padded_cols = input.cols();
    const int rows = padded_rows - 2;
    const int cols = padded_cols - 2;

    output.resize(rows, cols);

    // Use memcpy for each row - much faster than element-by-element copying
    for (int i = 0; i < rows; ++i) {
        std::memcpy(&output(i, 0), &input(i + 1, 1), cols * sizeof(uchar));
    }
}

void SobelOperator::applyKernel(const FlatImage& input, FlatImage& output, int rows, int cols) const {
    PROF_EXEC_TIME;

    output.resize(rows, cols); // Initialize to 0

    std::vector<int> row_indices(rows - 2);
    std::iota(row_indices.begin(), row_indices.end(), 1); // [1, 2, ..., rows-2]

    std::for_each(std::execution::par_unseq, row_indices.begin(), row_indices.end(), [&](int idxi) {
        for (int idxj = 1; idxj < cols - 1; ++idxj) {
            int gx = 0;
            int gy = 0;

            // Unrolled access
            gx += input(idxi - 1, idxj - 1) * SobelOperator::KERNELX[0][0];
            gx += input(idxi - 1, idxj    ) * SobelOperator::KERNELX[0][1];
            gx += input(idxi - 1, idxj + 1) * SobelOperator::KERNELX[0][2];

            gx += input(idxi    , idxj - 1) * SobelOperator::KERNELX[1][0];
            gx += input(idxi    , idxj    ) * SobelOperator::KERNELX[1][1];
            gx += input(idxi    , idxj + 1) * SobelOperator::KERNELX[1][2];

            gx += input(idxi + 1, idxj - 1) * SobelOperator::KERNELX[2][0];
            gx += input(idxi + 1, idxj    ) * SobelOperator::KERNELX[2][1];
            gx += input(idxi + 1, idxj + 1) * SobelOperator::KERNELX[2][2];

            gy += input(idxi - 1, idxj - 1) * SobelOperator::KERNELY[0][0];
            gy += input(idxi - 1, idxj    ) * SobelOperator::KERNELY[0][1];
            gy += input(idxi - 1, idxj + 1) * SobelOperator::KERNELY[0][2];

            gy += input(idxi    , idxj - 1) * SobelOperator::KERNELY[1][0];
            gy += input(idxi    , idxj    ) * SobelOperator::KERNELY[1][1];
            gy += input(idxi    , idxj + 1) * SobelOperator::KERNELY[1][2];

            gy += input(idxi + 1, idxj - 1) * SobelOperator::KERNELY[2][0];
            gy += input(idxi + 1, idxj    ) * SobelOperator::KERNELY[2][1];
            gy += input(idxi + 1, idxj + 1) * SobelOperator::KERNELY[2][2];

            int gradient = static_cast<int>((std::abs(gx) + std::abs(gy)) * NORMALIZATION_FACTOR);

            if (gradient < NORMALIZED_GRADIENT_THRESHOLD) {
                gradient = 0;
            }

            output(idxi, idxj) = std::clamp(gradient, 0, 255);
        }
    });
}

void SobelOperator::apply(const FlatImage& input, FlatImage& output) const {
    PROF_EXEC_TIME;

    FlatImage paddedImage;
    int padded_rows, padded_cols;
    padBoundaries(input, paddedImage, padded_rows, padded_cols);

    FlatImage convOutput;
    applyKernel(paddedImage, convOutput, padded_rows, padded_cols);

    removeBoundaries(convOutput, output);
}

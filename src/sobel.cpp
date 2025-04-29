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
    int rows = input.rows();
    int cols = input.cols();

    padded_rows = rows + 2;
    padded_cols = cols + 2;

    output.resize(padded_rows, padded_cols);

    // Iterate over original pixels
    std::for_each(std::execution::par_unseq, input.begin(), input.end(), [&](const uchar& pixel) {
        int idx = &pixel - &input[0];
        int i = idx / cols;
        int j = idx % cols;

        // Set center pixel
        output(i + 1, j + 1) = pixel;

        // Set padding top and bottom rows
        if (i == 0) {
            output(0, j + 1) = pixel;
            output(rows + 1, j + 1) = pixel;
        }

        // Set padding left and right columns
        if (j == 0) {
            output(i + 1, 0) = pixel;
            output(i + 1, cols + 1) = pixel;
        }

        // Set corners
        if (i == 0 && j == 0) {
            output(0, 0) = pixel;
            output(0, cols + 1) = pixel;
            output(rows + 1, 0) = pixel;
            output(rows + 1, cols + 1) = pixel;
        }
    });
}

void removeBoundaries(const FlatImage& input, FlatImage& output) {
    int rows = input.rows() - 2;
    int cols = input.cols() - 2;

    output.resize(rows, cols);

    std::for_each(std::execution::par_unseq, input.begin(), input.end(), [&](const uchar& pixel) {
        int idx = &pixel - &input[0];
        int i = idx / input.cols();
        int j = idx % input.cols();

        if (i > 0 && i < rows + 1 && j > 0 && j < cols + 1) {
            output(i - 1, j - 1) = pixel;
        }
    });
}

void SobelOperator::applyKernel(const FlatImage& input, FlatImage& output, int rows, int cols) const {
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

    cv::Mat customResultMat(paddedImage.rows(), paddedImage.cols(), CV_8UC1, const_cast<uchar*>(paddedImage.data().data()));
    cv::imshow("Padded Image", customResultMat);
    cv::waitKey(0);

    FlatImage convOutput;
    applyKernel(paddedImage, convOutput, padded_rows, padded_cols);

    cv::Mat d(convOutput.rows(), convOutput.cols(), CV_8UC1, const_cast<uchar*>(convOutput.data().data()));
    cv::imshow("d", d);
    cv::waitKey(0);

    removeBoundaries(convOutput, output);

    cv::Mat s(output.rows(), output.cols(), CV_8UC1, const_cast<uchar*>(output.data().data()));
    cv::imshow("s", s);
    cv::waitKey(0);
}

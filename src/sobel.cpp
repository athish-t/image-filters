#include <execution>

#include "prof_utils.hpp"
#include "sobel.hpp"

constexpr int NORMALIZED_GRADIENT_THRESHOLD = 50;
constexpr float NORMALIZATION_FACTOR = 0.5;

void SobelOperator::applyWithBenchmark(cv::Mat& output) {
    PROF_EXEC_TIME;

    cv::Mat greyImage;
    cv::cvtColor(_inputImage, greyImage, cv::COLOR_BGR2GRAY);
    cv::Sobel(greyImage, output, CV_8U, 1, 1);
}

inline uchar convertPixelToGreyScale(cv::Vec3b pixel) {
    return static_cast<uchar>(0.299 * pixel[2] + 0.587 * pixel[1] + 0.114 * pixel[0]);
}

void convertFlatToCvMat(const FlatArray<uchar>& input, cv::Mat& output, int rows, int cols, int type) {
    output = cv::Mat(rows, cols, type);

    std::for_each(std::execution::par_unseq, input.begin(), input.end(), [&](const uchar& value) {
        int idx = &value - &input[0];
        int i = idx / cols;
        int j = idx % cols;
        output.at<uchar>(i, j) = value;
    });
}

void convertToPaddedFlatArray(const cv::Mat& input, FlatArray<uchar>& output, int& padded_rows, int& padded_cols) {
    int rows = input.rows;
    int cols = input.cols;

    padded_rows = rows + 2;
    padded_cols = cols + 2;

    output.resize(padded_rows, padded_cols);

    // Iterate over original pixels
    std::for_each(std::execution::par_unseq, input.begin<cv::Vec3b>(), input.end<cv::Vec3b>(), [&](const cv::Vec3b& pixel) {
        int idx = &pixel - input.ptr<cv::Vec3b>();
        int i = idx / cols;
        int j = idx % cols;

        uchar greyValue = convertPixelToGreyScale(pixel);

        // Set center pixel
        output(i + 1, j + 1) = greyValue;

        // Set padding top and bottom rows
        if (i == 0) {
            output(0, j + 1) = greyValue;
            output(rows + 1, j + 1) = greyValue;
        }

        // Set padding left and right columns
        if (j == 0) {
            output(i + 1, 0) = greyValue;
            output(i + 1, cols + 1) = greyValue;
        }

        // Set corners
        if (i == 0 && j == 0) {
            output(0, 0) = greyValue;
            output(0, cols + 1) = greyValue;
            output(rows + 1, 0) = greyValue;
            output(rows + 1, cols + 1) = greyValue;
        }
    });
}

void SobelOperator::applyKernel(const FlatArray<uchar>& input, FlatArray<uchar>& output, int rows, int cols) {
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

void SobelOperator::apply(cv::Mat& output) {
    PROF_EXEC_TIME;

    // Step 1: Convert input image to flat padded array
    FlatArray<uchar> paddedInput;
    int padded_rows, padded_cols;
    convertToPaddedFlatArray(_inputImage, paddedInput, padded_rows, padded_cols);

    // Step 2: Create flat output array
    FlatArray<uchar> outputFlat;
    applyKernel(paddedInput, outputFlat, padded_rows, padded_cols);

    // Step 3: Convert flat output back to cv::Mat (removing padding)
    convertFlatToCvMat(outputFlat, output, padded_rows, padded_cols, CV_8UC1);
}

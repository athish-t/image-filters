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

void convertToCvMat(const std::vector<std::vector<uchar>>& input, cv::Mat& output, int type) {
    output = cv::Mat(input.size(), input[0].size(), type);
    std::for_each(std::execution::par, input.begin(), input.end(), [&](const std::vector<uchar>& row) {
        int i = &row - &input[0];
        std::for_each(row.begin(), row.end(), [&](const uchar& value) {
            int j = &value - &row[0];
            output.at<uchar>(i, j) = value;
        });
    });
}

void convertToPadded2DArray(const cv::Mat& input, std::vector<std::vector<uchar>>& output) {
    int rows = input.rows;
    int cols = input.cols;
    output.resize(rows + 2, std::vector<uchar>(cols + 2));

    std::for_each(std::execution::par, input.begin<cv::Vec3b>(), input.end<cv::Vec3b>(), [&](const cv::Vec3b& pixel) {
        int idx = &pixel - input.ptr<cv::Vec3b>();
        int i = idx / cols;
        int j = idx % cols;
        uchar greyValue = convertPixelToGreyScale(pixel);

        // Assign greyscale value to the padded output
        output[i + 1][j + 1] = greyValue;

        // Handle padding for the first and last rows
        if (i == 0) {
            output[0][j + 1] = greyValue;
            output[rows + 1][j + 1] = greyValue;
        }

        // Handle padding for the first and last columns
        if (j == 0) {
            output[i + 1][0] = greyValue;
            output[i + 1][cols + 1] = greyValue;
        }

        // Handle corners
        if (i == 0 && j == 0) {
            output[0][0] = greyValue;
            output[0][cols + 1] = greyValue;
            output[rows + 1][0] = greyValue;
            output[rows + 1][cols + 1] = greyValue;
        }
    });
}

void SobelOperator::applyKernel(const std::vector<std::vector<uchar>>& input, std::vector<std::vector<uchar>>& output) {
    const int rows = input.size();
    const int cols = input[0].size();

    output.resize(rows, std::vector<uchar>(cols, 0)); // Initialize with zeros

    std::vector<int> row_indices(rows - 2);
    std::iota(row_indices.begin(), row_indices.end(), 1); // [1, 2, ..., rows-2]

    std::for_each(std::execution::par, row_indices.begin(), row_indices.end(), [&](int idxi) {
        for (int idxj = 1; idxj < cols - 1; ++idxj) {
            int gx = 0;
            int gy = 0;

            // Unroll the Sobel kernel manually for better performance
            gx += input[idxi - 1][idxj - 1] * SobelOperator::KERNELX[0][0];
            gx += input[idxi - 1][idxj    ] * SobelOperator::KERNELX[0][1];
            gx += input[idxi - 1][idxj + 1] * SobelOperator::KERNELX[0][2];
            gx += input[idxi    ][idxj - 1] * SobelOperator::KERNELX[1][0];
            gx += input[idxi    ][idxj    ] * SobelOperator::KERNELX[1][1];
            gx += input[idxi    ][idxj + 1] * SobelOperator::KERNELX[1][2];
            gx += input[idxi + 1][idxj - 1] * SobelOperator::KERNELX[2][0];
            gx += input[idxi + 1][idxj    ] * SobelOperator::KERNELX[2][1];
            gx += input[idxi + 1][idxj + 1] * SobelOperator::KERNELX[2][2];

            gy += input[idxi - 1][idxj - 1] * SobelOperator::KERNELY[0][0];
            gy += input[idxi - 1][idxj    ] * SobelOperator::KERNELY[0][1];
            gy += input[idxi - 1][idxj + 1] * SobelOperator::KERNELY[0][2];
            gy += input[idxi    ][idxj - 1] * SobelOperator::KERNELY[1][0];
            gy += input[idxi    ][idxj    ] * SobelOperator::KERNELY[1][1];
            gy += input[idxi    ][idxj + 1] * SobelOperator::KERNELY[1][2];
            gy += input[idxi + 1][idxj - 1] * SobelOperator::KERNELY[2][0];
            gy += input[idxi + 1][idxj    ] * SobelOperator::KERNELY[2][1];
            gy += input[idxi + 1][idxj + 1] * SobelOperator::KERNELY[2][2];

            int gradient = static_cast<int>((std::abs(gx) + std::abs(gy)) * NORMALIZATION_FACTOR);

            if (gradient < NORMALIZED_GRADIENT_THRESHOLD) {
                gradient = 0;
            }

            output[idxi][idxj] = std::clamp(gradient, 0, 255);
        }
    });
}

void SobelOperator::apply(cv::Mat& output) {
    PROF_EXEC_TIME;

    std::vector<std::vector<uchar>> paddedInput;
    convertToPadded2DArray(_inputImage, paddedInput);
    std::vector<std::vector<uchar>> output2D;
    applyKernel(paddedInput, output2D);

    convertToCvMat(output2D, output, CV_8UC1);
}

#include <execution>

#include "prof_utils.hpp"
#include "sobel.hpp"

void SobelOperator::applyWithBenchmark(cv::Mat& output) {
    PROF_EXEC_TIME;

    cv::Mat greyImage;
    cv::cvtColor(_inputImage, greyImage, cv::COLOR_BGR2GRAY);
    cv::Sobel(greyImage, output, CV_8U, 1, 1);
}

uchar convertPixelToGreyScale(cv::Vec3b pixel) {
    return static_cast<uchar>(0.299 * pixel[2] + 0.587 * pixel[1] + 0.114 * pixel[0]);
}

void convertTo2DArray(const cv::Mat& input, std::vector<std::vector<uchar>>& output) {
    output.resize(input.rows, std::vector<uchar>(input.cols));
    std::for_each(std::execution::par, input.begin<cv::Vec3b>(), input.end<cv::Vec3b>(), [&](const cv::Vec3b& pixel) {
        int idx = &pixel - input.ptr<cv::Vec3b>();
        int i = idx / input.cols;
        int j = idx % input.cols;
        output[i][j] = convertPixelToGreyScale(pixel);
    });
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

void padBounderies(std::vector<std::vector<uchar>>& input) {
    int rows = input.size();
    int cols = input[0].size();
    std::vector<std::vector<uchar>> padded(rows + 2, std::vector<uchar>(cols + 2));

    std::for_each(std::execution::par, input.begin(), input.end(), [&](const std::vector<uchar>& row) {
        int i = &row - &input[0];
        std::for_each(row.begin(), row.end(), [&](const uchar& value) {
            int j = &value - &row[0];
            padded[i + 1][j + 1] = value;
        });
    });

    std::for_each(std::execution::par, padded.begin(), padded.end(), [&](std::vector<uchar>& row) {
        int i = &row - &padded[0];
        if (i == 0 || i == rows + 1) return;
        row[0] = row[1];
        row[cols + 1] = row[cols];
    });

    std::for_each(std::execution::par, padded[0].begin(), padded[0].end(), [&](uchar& value) {
        int j = &value - &padded[0][0];
        padded[0][j] = padded[1][j];
        padded[rows + 1][j] = padded[rows][j];
    });

    input.swap(padded);
}

void SobelOperator::applyKernel(const std::vector<std::vector<uchar>>& input, std::vector<std::vector<uchar>>& output) {
    int rows = input.size();
    int cols = input[0].size();
    output.resize(rows, std::vector<uchar>(cols));

    std::for_each(std::execution::par, input.begin() + 1, input.end() - 1, [&](const std::vector<uchar>& row) {
        int idxi = &row - &input[0];
        std::for_each(row.begin() + 1, row.end() - 1, [&](const uchar& value) {
            int idxj = &value - &row[0];
            int gx = 0;
            int gy = 0;

            for (int kx = -1; kx <= 1; ++kx) {
                for (int ky = -1; ky <= 1; ++ky) {
                    gx += SobelOperator::KERNELX[kx + 1][ky + 1] * input[idxi + kx][idxj + ky];
                    gy += SobelOperator::KERNELY[kx + 1][ky + 1] * input[idxi + kx][idxj + ky];
                }
            }

            int g = static_cast<int>(std::sqrt(gx * gx + gy * gy));
            output[idxi][idxj] = static_cast<uchar>(g * 255 / std::sqrt(2 * 255 * 255));
        });
    });
}

void SobelOperator::apply(cv::Mat& output) {
    PROF_EXEC_TIME;

    std::vector<std::vector<uchar>> input;
    convertTo2DArray(_inputImage, input);
    padBounderies(input);
    std::vector<std::vector<uchar>> output2D;
    applyKernel(input, output2D);

    convertToCvMat(output2D, output, CV_8UC1);
}

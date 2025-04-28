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
    for (int i = 0; i < input.rows; ++i) {
        for (int j = 0; j < input.cols; ++j) {
            output[i][j] = convertPixelToGreyScale(input.at<cv::Vec3b>(i, j));
        }
    }
}

void convertToCvMat(const std::vector<std::vector<uchar>>& input, cv::Mat& output, int type) {
    output = cv::Mat(input.size(), input[0].size(), type);
    for (int i = 0; i < input.size(); ++i) {
        for (int j = 0; j < input[i].size(); ++j) {
            output.at<uchar>(i, j) = input[i][j];
        }
    }
}

void padBounderies(std::vector<std::vector<uchar>>& input) {
    int rows = input.size();
    int cols = input[0].size();
    std::vector<std::vector<uchar>> padded(rows + 2, std::vector<uchar>(cols + 2));

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            padded[i + 1][j + 1] = input[i][j];
        }
    }

    for (int i = 0; i < rows + 2; ++i) {
        padded[i][0] = padded[i][1];
        padded[i][cols + 1] = padded[i][cols];
    }

    for (int j = 0; j < cols + 2; ++j) {
        padded[0][j] = padded[1][j];
        padded[rows + 1][j] = padded[rows][j];
    }

    input.swap(padded);
}

void applyKernel(const std::vector<std::vector<uchar>>& input, std::vector<std::vector<uchar>>& output) {
    int rows = input.size();
    int cols = input[0].size();
    output.resize(rows, std::vector<uchar>(cols));

    for (int i = 1; i < rows - 1; ++i) {
        for (int j = 1; j < cols - 1; ++j) {
            int gx = 0;
            int gy = 0;

            for (int kx = -1; kx <= 1; ++kx) {
                for (int ky = -1; ky <= 1; ++ky) {
                    gx += SobelOperator::KERNELX[kx + 1][ky + 1] * input[i + kx][j + ky];
                    gy += SobelOperator::KERNELY[kx + 1][ky + 1] * input[i + kx][j + ky];
                }
            }

            int g = static_cast<int>(std::sqrt(gx * gx + gy * gy));
            output[i][j] = static_cast<uchar>(g * 255 / std::sqrt(2 * 255 * 255));
        }
    }
}

void SobelOperator::apply(cv::Mat& output) {
    PROF_EXEC_TIME;

    std::vector<std::vector<uchar>> input;
    convertTo2DArray(_inputImage, input);
    padBounderies(input);
    std::vector<std::vector<uchar>> output2D;
    applyKernel(input, output2D);

    convertToCvMat(output2D, output, CV_8UC1);
    // cv::imshow("test", output);
    // cv::waitKey(0);
}

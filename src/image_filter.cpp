#include <execution>

#include "image_filter.hpp"
#include "prof_utils.hpp"
#include "types.hpp"
#include <opencv2/opencv.hpp>


constexpr float NORMALIZATION_FACTOR = 0.5;


std::pair<int, int> ImageFilter::padBoundaries(const FlatImage& input, FlatImage& output) {
    PROF_EXEC_TIME;

    const int rows = input.rows();
    const int cols = input.cols();

    int padded_rows = rows + 2;
    int padded_cols = cols + 2;

    output.resize(padded_rows, padded_cols);

    // copy the inner content
    for (int i = 0; i < rows; ++i) {
        std::memcpy(&output(i + 1, 1), &input(i, 0), cols * sizeof(uchar));
    }

    // top and bottom edge rows (without corners)
    std::memcpy(&output(0, 1), &input(0, 0), cols * sizeof(uchar));
    std::memcpy(&output(padded_rows - 1, 1), &input(rows - 1, 0), cols * sizeof(uchar));

    // left and right edge columns
    for (int i = 0; i < rows; ++i) {
        output(i + 1, 0) = input(i, 0);
        output(i + 1, padded_cols - 1) = input(i, cols - 1);
    }

    // four corners
    output(0, 0) = input(0, 0);                             // Top-left
    output(0, padded_cols - 1) = input(0, cols - 1);        // Top-right
    output(padded_rows - 1, 0) = input(rows - 1, 0);        // Bottom-left
    output(padded_rows - 1, padded_cols - 1) = input(rows - 1, cols - 1); // Bottom-right

    return {padded_rows, padded_cols};
}


void ImageFilter::removeBoundaries(const FlatImage& input, FlatImage& output) {
    PROF_EXEC_TIME;

    const int padded_rows = input.rows();
    const int padded_cols = input.cols();
    const int rows = padded_rows - 2;
    const int cols = padded_cols - 2;

    output.resize(rows, cols);

    for (int i = 0; i < rows; ++i) {
        std::memcpy(&output(i, 0), &input(i + 1, 1), cols * sizeof(uchar));
    }
}

template <typename KType>
void ImageFilter::getGradient(const FlatImage& input, FlatImage& output, int padded_rows, int padded_cols, const KType kernel[3][3], uchar threshold) {
    PROF_EXEC_TIME;

    output.resize(padded_rows, padded_cols);

    std::vector<int> row_indices(padded_rows - 2);
    std::iota(row_indices.begin(), row_indices.end(), 1); // [1, 2, ..., padded_rows-2]

    std::for_each(std::execution::par_unseq, row_indices.begin(), row_indices.end(), [&](int idxi) {
        for (int idxj = 1; idxj < padded_cols - 1; ++idxj) {
            int gradient = 0;

            gradient += input(idxi - 1, idxj - 1) * kernel[0][0];
            gradient += input(idxi - 1, idxj    ) * kernel[0][1];
            gradient += input(idxi - 1, idxj + 1) * kernel[0][2];

            gradient += input(idxi    , idxj - 1) * kernel[1][0];
            gradient += input(idxi    , idxj    ) * kernel[1][1];
            gradient += input(idxi    , idxj + 1) * kernel[1][2];

            gradient += input(idxi + 1, idxj - 1) * kernel[2][0];
            gradient += input(idxi + 1, idxj    ) * kernel[2][1];
            gradient += input(idxi + 1, idxj + 1) * kernel[2][2];

            output(idxi, idxj) =  std::abs(gradient) < threshold ? 0 : std::clamp(std::abs(gradient), 0, 255);
        }
    });
}

void ImageFilter::combineGradients(const FlatImage& gx, const FlatImage& gy, FlatImage& combinedGradient, uchar threshold) {
    assert(gx.rows() == gy.rows() && gx.cols() == gy.cols());
    int rows = gx.rows();
    int cols = gx.cols();

    PROF_EXEC_TIME;

    combinedGradient.resize(rows, cols);

    std::vector<int> row_indices(rows);
    std::iota(row_indices.begin(), row_indices.end(), 0); // [0, 1, 2, ..., rows - 1]

    std::for_each(std::execution::par_unseq, row_indices.begin(), row_indices.end(), [&](int idxi) {
        for (int idxj = 0; idxj < cols; ++idxj) {
            int gradient = static_cast<int>((gx(idxi, idxj) + gy(idxi, idxj)) * NORMALIZATION_FACTOR);
            combinedGradient(idxi, idxj) = gradient < threshold ? 0 : std::clamp(gradient, 0, 255);
        }
    });
}

template <typename KType>
void ImageFilter::applyXYKernels(const FlatImage& input, FlatImage& output, const KType kernelX[3][3], const KType kernelY[3][3], uchar threshold) const {
    FlatImage paddedImage;
    auto [padded_rows, padded_cols] = padBoundaries(input, paddedImage);

    FlatImage gx, gy;
    getGradient(paddedImage, gx, padded_rows, padded_cols, kernelX);
    getGradient(paddedImage, gy, padded_rows, padded_cols, kernelY);

    FlatImage combinedGradient;
    combineGradients(gx, gy, combinedGradient, threshold);

    removeBoundaries(combinedGradient, output);
}

template <typename KType>
void ImageFilter::applySingleKernel(const FlatImage& input, FlatImage& output, const KType kernel[3][3], uchar threshold) const {
    FlatImage paddedImage;
    auto [padded_rows, padded_cols] = padBoundaries(input, paddedImage);

    FlatImage gradient;
    getGradient(paddedImage, gradient, padded_rows, padded_cols, kernel, threshold);

    removeBoundaries(gradient, output);
}

// Explicit template instantiation
template void ImageFilter::applyXYKernels(const FlatImage& input, FlatImage& output, const int kernelX[3][3], const int kernelY[3][3], uchar threshold) const;
template void ImageFilter::applyXYKernels(const FlatImage& input, FlatImage& output, const float kernelX[3][3], const float kernelY[3][3], uchar threshold) const;
template void ImageFilter::applySingleKernel(const FlatImage& input, FlatImage& output, const int kernel[3][3], uchar threshold) const;
template void ImageFilter::applySingleKernel(const FlatImage& input, FlatImage& output, const float kernel[3][3], uchar threshold) const;
template void ImageFilter::getGradient(const FlatImage& input, FlatImage& output, int padded_rows, int padded_cols, const int kernel[3][3], uchar threshold);
template void ImageFilter::getGradient(const FlatImage& input, FlatImage& output, int padded_rows, int padded_cols, const float kernel[3][3], uchar threshold);

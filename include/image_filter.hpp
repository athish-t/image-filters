# pragma once

#include <opencv2/opencv.hpp>
#include "types.hpp"

class ImageFilter
{
public:
    virtual void apply(const FlatImage& input, FlatImage& output) const = 0;
    virtual void applyBenchmark(const cv::Mat& input, cv::Mat& output) const = 0;

    virtual ~ImageFilter() = default;

protected:
    void applyXYkernels(const FlatImage& input, FlatImage& output, const int kernelX[3][3], const int kernelY[3][3]) const;
    void applySingleKernel(const FlatImage& input, FlatImage& output, const int kernel[3][3]) const;

    static std::pair<int, int> padBoundaries(const FlatImage& input, FlatImage& output);
    static void removeBoundaries(const FlatImage& input, FlatImage& output);
    static void getGradient(const FlatImage& input, FlatImage& output, int padded_rows, int padded_cols, const int kernel[3][3]);
    static void combineGradients(const FlatImage& gx, const FlatImage& gy, FlatImage& combinedGradient, int padded_rows, int padded_cols);

};

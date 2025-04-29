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
    template <typename KType>
    void applyXYkernels(const FlatImage& input, FlatImage& output, const KType kernelX[3][3], const KType kernelY[3][3], uchar threshold = 0) const;
    template <typename KType>
    void applySingleKernel(const FlatImage& input, FlatImage& output, const KType kernel[3][3], uchar threshold = 0) const;

    static std::pair<int, int> padBoundaries(const FlatImage& input, FlatImage& output);
    static void removeBoundaries(const FlatImage& input, FlatImage& output);
    template <typename KType> static void getGradient(const FlatImage& input, FlatImage& output, int padded_rows, int padded_cols, const KType kernel[3][3], uchar threshold = 0);
    static void combineGradients(const FlatImage& gx, const FlatImage& gy, FlatImage& combinedGradient, uchar threshold = 0);

};

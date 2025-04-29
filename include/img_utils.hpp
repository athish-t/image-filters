#pragma once

#include "types.hpp"

template <typename T>
inline uchar convertPixelToGreyScale(const T& pixel) {
    return static_cast<uchar>(0.299 * pixel[2] + 0.587 * pixel[1] + 0.114 * pixel[0]);
}

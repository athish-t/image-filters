#include <vector>
#include "types.hpp"

FlatImage createTestImage() {
    FlatImage image(5, 5); // Create a 5x5 test image
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            image(i, j) = static_cast<uchar>((i + j) % 256); // Fill with sample values
        }
    }
    return image;
}

FlatImage createTestImageWithPadding() {
    FlatImage image(7, 7); // Create a 7x7 padded test image
    for (int i = 0; i < 7; ++i) {
        for (int j = 0; j < 7; ++j) {
            if (i == 0 || i == 6 || j == 0 || j == 6) {
                image(i, j) = 0; // Padding with zeros
            } else {
                image(i, j) = static_cast<uchar>(((i - 1) + (j - 1)) % 256); // Inner values
            }
        }
    }
    return image;
}

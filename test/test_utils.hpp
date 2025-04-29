#include <vector>
#include "types.hpp"

FlatImage createTestImage(int rows, int cols, std::optional<uchar> value = std::nullopt) {
    FlatImage image(5, 5); // Create a 5x5 test image
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            image(i, j) = value ? *value : static_cast<uchar>((i + j) % 256);
        }
    }
    return image;
}

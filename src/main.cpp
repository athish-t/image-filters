#include "cv_utils.hpp"
#include "sobel.hpp"

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <input_image_path> <output_image_path>" << std::endl;
        return -1;
    }

    std::string inputImagePath = argv[1];
    std::string outputImagePath = argv[2];
    cv::Mat image = cv::imread(inputImagePath);

    if (image.empty()) {
        std::cerr << "Failed to open the image at: " << inputImagePath << std::endl;
        return -1;
    }

    cv::Mat benchmarkResult;
    cv::Mat customResult;

    auto op = std::make_unique<SobelOperator>(image);
    op->applyWithBenchmark(benchmarkResult);
    op->apply(customResult);

    displayImages({std::cref(image), std::cref(benchmarkResult), std::cref(customResult)});
    writeImage(outputImagePath, customResult);

    return 0;
}

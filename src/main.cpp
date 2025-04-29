#include "io_utils.hpp"
#include "filter_pipeline.hpp"
#include "sobel.hpp"

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <input_image_path> <output_image_path>" << std::endl;
        return -1;
    }

    std::string inputImagePath = argv[1];
    std::string outputImagePath = argv[2];
    cv::Mat image = cv::imread(inputImagePath, cv::IMREAD_GRAYSCALE);

    if (image.empty()) {
        std::cerr << "Failed to open the image at: " << inputImagePath << std::endl;
        return -1;
    }

    auto pipeline = FilterPipeline().add(std::make_shared<SobelOperator>());

    cv::Mat benchmarkResult;
    pipeline
    .applyBenchmark(image, benchmarkResult);

    FlatImage customResult;
    pipeline
    .apply(FlatImageFactory::from(image), customResult);

    cv::Mat customResultMat(customResult.rows(), customResult.cols(), CV_8UC1, const_cast<uchar*>(customResult.data().data()));
    displayImages({std::cref(image), std::cref(benchmarkResult), std::cref(customResultMat)});
    writeImage(outputImagePath, customResultMat);

    return 0;
}

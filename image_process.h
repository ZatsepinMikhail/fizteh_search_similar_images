#include <opencv2/opencv.hpp>
#include <boost/filesystem.hpp>
#include <string>

const size_t kDefaultHeight = 20;
const size_t kDefaultWidth = 20;

void ReadAndWriteCompressed(const std::string from_path, const std::string to_path) {
    cv::Mat input_image = cv::imread(from_path.c_str());
    cv::Mat compressed_image;
    cv::resize(input_image, compressed_image, cv::Size(kDefaultHeight, kDefaultWidth), 0, 0);
    cv::imwrite(to_path.c_str(), compressed_image);
}
#include <opencv2/opencv.hpp>
#include <cmath>
#include <iostream>

#include <stdexcept>

class VectorizedThumbnail;

class Thumbnail {
public:
	Thumbnail(cv::Mat image, const char* source_image_path)
	  : image_(image),
		source_image_path_(source_image_path) {
			//std::cout << source_image_path << "\n";;
		}

	const char* GetSourceImagePath() const {
		return source_image_path_;
	}

	cv::Size GetImageSize() const {
		return image_.size();
	}

	size_t GetImageWidth() const {
		return image_.size().width;
	}

	size_t GetImageHeight() const {
		return image_.size().height;
	}
private:
	cv::Mat image_;
	const char* source_image_path_;

	friend class VectorizedThumbnail;
};

class VectorizedThumbnail {
public:
	VectorizedThumbnail(Thumbnail& thumbnail)
	  : parent_thumbnail_(thumbnail),
	    vectorized_image_(3 * thumbnail.GetImageHeight() * thumbnail.GetImageWidth()) {
	    size_t curr_index = 0;
	    for (size_t i = 0; i < thumbnail.GetImageHeight(); ++i) {
	    	for (size_t j = 0; j < thumbnail.GetImageWidth(); ++j) {
	    		cv::Vec3b colour = thumbnail.image_.at<cv::Vec3b>(i, j);
	    		vectorized_image_[curr_index] = colour[0];
	    		vectorized_image_[curr_index + 1] = colour[1];
	    		vectorized_image_[curr_index + 2] = colour[2];
	    		curr_index += 3;
	    	}
	    }
	    for (unsigned char element : vectorized_image_) {
	    	module_ += element * element;
	    }
	    module_ = std::sqrt(module_);
	}	

	size_t Size() const {
		return vectorized_image_.size();
	}

	double GetModule() const {
		return module_;
	}

	//scalar production
	double operator * (const VectorizedThumbnail& other) {
		if (Size() != other.Size()) {
			throw std::runtime_error("scalar production is undefined");
		}
		double result = 0;
		for (size_t i = 0; i < Size(); ++i) {
			result += vectorized_image_[i] * other.vectorized_image_[i];
		}
		return result;
	}

	Thumbnail parent_thumbnail_;
	std::vector<unsigned char> vectorized_image_;

	//rename
	double module_;
};
#pragma once

#include <opencv2/opencv.hpp>
#include <cmath>
#include <iostream>
#include <stdexcept>

#include "constants.h"

class VectorizedThumbnail {
public:
	VectorizedThumbnail(cv::Mat image, const char* source_image_path)
	  : vectorized_image_(kDefaultDimensionality),
	    source_image_path_(source_image_path) {
    	cv::Mat compressed_image;
    	cv::resize(image, compressed_image, cv::Size(kDefaultHeight, kDefaultWidth), 0, 0);

    	//naming for private methods?
    	init_vectorized_image(compressed_image);
	}

	bool operator == (const VectorizedThumbnail& other) const {
		return source_image_path_ == other.source_image_path_;
	}

	const char* get_source_image_path() const {
		return source_image_path_;
	}

	std::vector<char> vectorized_image_;
	const char* source_image_path_;

private:

	void init_vectorized_image(const cv::Mat& image) {
		size_t curr_index = 0;
	
	  for (int i = 0; i < image.rows; ++i) {
	  	for (int j = 0; j < image.cols; ++j) {
	  		cv::Vec3b colour = image.at<cv::Vec3b>(i, j);
	   		vectorized_image_[curr_index] = colour[0] + kRangeLeft;
	   		vectorized_image_[curr_index + 1] = colour[1] + kRangeLeft;
	   		vectorized_image_[curr_index + 2] = colour[2] + kRangeLeft;
	   		curr_index += 3;
	   	}
	  }
	}

};
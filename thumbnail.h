#pragma once

#include <opencv2/opencv.hpp>
#include <cmath>
#include <iostream>
#include <stdexcept>

#include "constants.h"

class VectorizedThumbnail {
public:
	//tmp
	VectorizedThumbnail(){}
	
	VectorizedThumbnail(cv::Mat image, const char* source_path)
	  : vectorized_image_(kDefaultDimensionality),
	    source_path_(source_path) {
    	cv::Mat compressed_image;
    	cv::resize(image, compressed_image, cv::Size(kDefaultHeight, kDefaultWidth), 0, 0);
    	cv::Mat gray_image;
    	cv::cvtColor(compressed_image, gray_image, CV_BGR2GRAY);
    	//naming for private methods?
    	init_vectorized_image(gray_image);
	}

	VectorizedThumbnail(const VectorizedThumbnail& other) {
		source_path_ = other.source_path_;
		vectorized_image_ = other.vectorized_image_;
	}

	bool operator == (const VectorizedThumbnail& other) const {
		return source_path_ == other.source_path_;
	}

	bool operator != (const VectorizedThumbnail& other) const {
		return source_path_ != other.source_path_;
	}

	const char* get_source_path() const {
		return source_path_;
	}

	std::vector<short> vectorized_image_;
	const char* source_path_;

	//write destructor

private:

	void init_vectorized_image(const cv::Mat& image) {
		size_t curr_index = 0;
	
	  for (int i = 0; i < image.rows; ++i) {
	  	for (int j = 0; j < image.cols; ++j) {
	  		/*cv::Vec3b colour = image.at<cv::Vec3b>(i, j);
	   		vectorized_image_[curr_index] = colour[0];
	   		vectorized_image_[curr_index + 1] = colour[1];
	   		vectorized_image_[curr_index + 2] = colour[2];
	   		curr_index += 3;*/
	   		unsigned char colour = image.at<unsigned char>(i, j);
	   		vectorized_image_[curr_index] = colour;
	   		++curr_index;
	   	}
	  }
	}

};


double GetCosBtwVectors(const VectorizedThumbnail& first, const VectorizedThumbnail& second) {
	const std::vector<short> first_vector = first.vectorized_image_;
	const std::vector<short> second_vector = second.vectorized_image_;

	double scalar_product = 0.f;
	double first_module = 0.f;
	double second_module = 0.f;

	for (size_t curr_pos = 0; curr_pos < kDefaultDimensionality; ++curr_pos) {
		scalar_product += first_vector[curr_pos] * second_vector[curr_pos];
		first_module += first_vector[curr_pos] * first_vector[curr_pos];
		second_module += second_vector[curr_pos] * second_vector[curr_pos];
	}
	return scalar_product / std::sqrt(first_module * second_module);
}

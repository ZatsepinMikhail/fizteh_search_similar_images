#include <opencv2/opencv.hpp>

#include <bitset>
#include <vector>
#include <iostream>

const size_t kDefaultHeight = 32;
const size_t kDefaultWidth = 32;

#include "thumbnail.h"

size_t threshold;

std::bitset<kDefaultWidth * kDefaultHeight / 16> GetDCTHash(Thumbnail thumb) {
	cv::Mat image = thumb.image_;
	cv::cvtColor(image, image, CV_BGR2GRAY);
	
	image.convertTo(image, CV_32FC1);
	cv::Mat dct_image;
	cv::dct(image, dct_image);

	//[0][0] contains unsuitable inforamtion
	float sum = 0.f;

	for (int i = 1; i < dct_image.rows / 4 + 1; ++i) {
		for (int j = 1; j < dct_image.cols / 4 + 1; ++j) {
			sum += dct_image.at<float>(i, j);
		}
	}

	float average = 16 * sum / (dct_image.rows * dct_image.cols);

	std::bitset<kDefaultWidth * kDefaultHeight / 16> hash;
	size_t curr_pos = 0;
	for (int i = 1; i < dct_image.rows / 4 + 1; ++i) {
		for (int j = 1; j < dct_image.cols / 4 + 1; ++j) {
			if (dct_image.at<float>(i, j) > average) {
				hash[curr_pos] = 1;
			} else {
				hash[curr_pos] = 0;
			}
			++curr_pos;
		}
	}

	return hash;
}


int GetHammingDistance(const std::bitset<kDefaultHeight * kDefaultWidth / 16>& first, 
	                      const std::bitset<kDefaultHeight * kDefaultHeight / 16>& second) {
	size_t different_bits_count = 0;
	for (size_t i = 0; i < kDefaultHeight * kDefaultHeight / 16; ++i) {
		if (first[i] != second[i]) {
			++different_bits_count;
		}
		if (different_bits_count > threshold) {
			return -1;
		}
	}
	return different_bits_count;
}
#include <opencv2/opencv.hpp>

#include <bitset>
#include <vector>
#include <iostream>

const size_t kDefaultHeight = 20;
const size_t kDefaultWidth = 20;

#include "thumbnail.h"

size_t threshold;

std::bitset<kDefaultWidth * kDefaultHeight> GetPerceptionHash(Thumbnail thumb) {
	cv::Mat image = thumb.image_;
	cv::Mat gray_image;
	cv::cvtColor(image, gray_image, CV_BGR2GRAY);
	//cv::Mat hashed_gray_image = gray_image.clone();
	unsigned result = 0;
	//cv::namedWindow( "compressed image", cv::WINDOW_AUTOSIZE );// Create a window for display.
   // cv::imshow( "compressed image", image );  
    //cv::namedWindow( "compressed gray image", cv::WINDOW_AUTOSIZE );// Create a window for display.
  //  cv::imshow( "compressed gray image", gray_image );  

	for (int i = 0; i < gray_image.rows; ++i) {
		for (int j = 0; j < gray_image.cols; ++j) {
			result += gray_image.at<unsigned char>(i, j);
		}
	}

	std::bitset<kDefaultWidth * kDefaultHeight> perceptive_hash;
	
	float average_colour = (float) result / (gray_image.rows * gray_image.cols);
	size_t curr_pos = 0;
	for (int i = 0; i < gray_image.rows; ++i) {
		for (int j = 0; j < gray_image.cols; ++j) {
			if (gray_image.at<unsigned char>(i, j) >= average_colour) {
				perceptive_hash[curr_pos] = 1;
			} else {
				perceptive_hash[curr_pos] = 0;
			}
			++curr_pos;
		}
	}

//	cv::namedWindow( "hashed compressed gray image", cv::WINDOW_AUTOSIZE );// Create a window for display.
//    cv::imshow( "hashed compressed gray image", hashed_gray_image );  
	return perceptive_hash;
}

size_t GetHammingDistance(const std::bitset<kDefaultHeight * kDefaultWidth>& first, 
	                      const std::bitset<kDefaultHeight * kDefaultHeight>& second) {
	size_t different_bits_count = 0;
	for (size_t i = 0; i < kDefaultHeight * kDefaultHeight; ++i) {
		if (first[i] != second[i]) {
			++different_bits_count;
		}
		if (different_bits_count > threshold) {
			return kDefaultHeight * kDefaultWidth;
		}
	}
	return different_bits_count;
}
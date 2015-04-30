#include <boost/filesystem.hpp>

#include <iostream>
#include <vector>

#include "thumbnail.h"

using namespace boost::filesystem;

using namespace std;

const string images_base = "/home/mikhail/image_search/pictures_base/";

//const string compressed_images = "/home/mikhail/image_search/compressed_images/";

const size_t kDefaultHeight = 200;
const size_t kDefaultWidth = 200;

Thumbnail CompressImage(std::string* from_path) {
    cv::Mat input_image = cv::imread(from_path->c_str());
    cv::Mat compressed_image;
    cv::resize(input_image, compressed_image, cv::Size(kDefaultHeight, kDefaultWidth), 0, 0);
    return Thumbnail(compressed_image, from_path->c_str());
}

int main() {

	path input_path(images_base);
	
    vector<Thumbnail> thumbnails;
    vector<VectorizedThumbnail> vectorized_thumbnails;

    //deprecated
    size_t count = 0;

	try {
	    if (exists(input_path)) {

    		if (is_regular_file(input_path)) {
        		cout << input_path << " size is " << file_size(input_path) << '\n';
    		} else if (is_directory(input_path)) {
        		
        		for (auto it = recursive_directory_iterator(input_path); it != recursive_directory_iterator(); ++it) {
        			if (is_regular_file(it->path())) {
                        thumbnails.push_back(CompressImage(new string(it->path().string())));
                        ++count;
                    }
        		}
        	} else {
        	cout << input_path << " exists, but is neither a regular file nor a directory\n";
        	}
        } else {
	    	cout << input_path << " does not exist\n";
		}
	} catch (const filesystem_error& ex) {
    	cout << ex.what() << '\n';
  	}
    
    for (Thumbnail thumb : thumbnails) {
        vectorized_thumbnails.push_back(VectorizedThumbnail(thumb));
    }
    
    cv::Mat cos_between_vectors(vectorized_thumbnails.size(), vectorized_thumbnails.size(), CV_64F);
    for (size_t i = 0; i < vectorized_thumbnails.size(); ++i) {
        for (size_t j = i + 1; j < vectorized_thumbnails.size(); ++j) {
            cos_between_vectors.at<double>(i, j) = (vectorized_thumbnails[i] * vectorized_thumbnails[j]) 
               / (vectorized_thumbnails[i].GetModule() * vectorized_thumbnails[j].GetModule());
        }
    }
    double max = 0;
    size_t index_first = 0;
    size_t index_second = 0;
    for (size_t i = 0; i < vectorized_thumbnails.size(); ++i) {
        for (size_t j = i + 1; j < vectorized_thumbnails.size(); ++j) {
            if (cos_between_vectors.at<double>(i, j) > max) {
                max = cos_between_vectors.at<double>(i, j);
                index_first = i;
                index_second = j;
            }
        }
    }
    cout << max << "\n";
    cout << index_first << "\n";
    cout << index_second << "\n";
    cout << vectorized_thumbnails[index_first].parent_thumbnail_.GetSourceImagePath() << "\n";
    cout << vectorized_thumbnails[index_second].parent_thumbnail_.GetSourceImagePath() << "\n";
  	return 0;
}
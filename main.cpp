#include <boost/filesystem.hpp>

#include <ctime>
#include <iostream>
#include <vector>
#include <unordered_set>

#include "thumbnail.h"
#include "hash_functions.h"

using namespace boost::filesystem;

using namespace std;

const string images_base = "/home/mikhail/image_search/pictures_base/N2";
//const string compressed_images = "/home/mikhail/image_search/compressed_images/";

VectorizedThumbnail GetThumbnailFromImage(std::string* from_path) {
  cv::Mat image = cv::imread(from_path->c_str());
  return VectorizedThumbnail(image, from_path->c_str());
}

void LoadImages(const string input_path, vector<VectorizedThumbnail>& loaded_images) {
  try {
    if (exists(input_path)) {

      if (is_directory(input_path)) {
                
        for (auto it = recursive_directory_iterator(input_path); it != recursive_directory_iterator(); ++it) {

          if (is_regular_file(it->path())) {                      
            loaded_images.push_back(GetThumbnailFromImage(new string(it->path().string())));
          }
        }
      
      } else {
        throw runtime_error("given path is not directory");    
      }

    } else {
      throw runtime_error("given path doesn't exist");
    }
  } catch (const filesystem_error& ex) {
    cout << ex.what() << '\n';
  }
}


int main() {

  path input_path(images_base);
	
  vector<VectorizedThumbnail> thumbnails;

  LoadImages(images_base, thumbnails);

  vector<unordered_multiset<VectorizedThumbnail, CompoundHashFunction>> hash_tables;
  for (size_t i = 0; i < kHashTableNumber; ++i) {
    hash_tables.emplace_back(thumbnails.begin(), thumbnails.end());
  }

  
  return 0;
}
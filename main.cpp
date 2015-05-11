#include <boost/filesystem.hpp>

#include <ctime>
#include <iostream>
#include <vector>
#include <unordered_set>

#include "thumbnail.h"
#include "hash_functions.h"

using namespace boost::filesystem;

using namespace std;

const string images_base = "/home/mikhail/image_search/pictures_base/OXC1/";
const string result_path = "/home/mikhail/image_search/project/sim_hash/request.jpeg";
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

  for (size_t curr_table = 0; curr_table < kHashTableNumber; ++curr_table) {
    size_t bucket_number = hash_tables[curr_table].bucket(thumbnails[0]);
    for (auto it = hash_tables[curr_table].begin(bucket_number); it != hash_tables[curr_table].end(bucket_number); ++it) {
      if (thumbnails[199] != *it) {
        float cos_btw_vectors = std::abs(GetCosBtwVectors(*it, thumbnails[199]));
        if (cos_btw_vectors >= kCRange) {
          cout << curr_table << ": with" << cos_btw_vectors << "..." << it->get_source_path() << "\n";
        }
      }
    }
  }

  return 0;
}
#include <boost/filesystem.hpp>

#include <map>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>

#include "steady_timer.h"
#include "thumbnail.h"
#include "hash_functions.h"
#include "thread_pool/task_queue.h"
#include "thread_pool/thread_guard.h"

using namespace boost::filesystem;

using namespace std;

const string images_base = "/home/mikhail/image_search/pictures_base/Altai";
const string result_path = "/home/mikhail/image_search/result/";

using HashTable = unordered_multiset<VectorizedThumbnail, CompoundHashFunction>;

VectorizedThumbnail GetThumbnailFromImage(std::string from_path) {
  cv::Mat image = cv::imread(from_path.c_str());
  return VectorizedThumbnail(image, from_path.c_str());
}

bool IsImage(const string extension) {
  return extension == ".jpg" || extension == ".bmp";
}

void LoadImages(const string input_path, vector<VectorizedThumbnail>& loaded_images) {
  TaskQueue<VectorizedThumbnail> task_queue(1000);
  try {
    if (exists(input_path)) {

      if (is_directory(input_path)) {
                
        for (auto it = recursive_directory_iterator(input_path); it != recursive_directory_iterator(); ++it) {
          if (is_regular_file(it->path())) {
            if (IsImage(it->path().extension().string())) {
              //loaded_images.push_back(GetThumbnailFromImage(*(new string(it->path().string()))));
              task_queue.Submit(std::bind(GetThumbnailFromImage, *(new string(it->path().string()))));
            }
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
  loaded_images = task_queue.ShutDown();
}

void InitializeHashTables(vector<HashTable>& hash_tables, vector<VectorizedThumbnail>& thumbnails, int threads_number, int id) {
  for (int curr_table = id; curr_table < hash_tables.size(); curr_table += threads_number) {
    hash_tables[curr_table].insert(thumbnails.begin(), thumbnails.end());
  }
}


int main() {
  
  path input_path(images_base);
  vector<VectorizedThumbnail> thumbnails;

  steady_timer timer;
  LoadImages(images_base, thumbnails);
  cout << timer.seconds_elapsed() << "\n";

  vector<HashTable> hash_tables;

  vector<thread> threads;

  int threads_number = GetDefaultNumberOfWorkers();
  timer.reset();
  //slower:
  /*for (int i = 0; i < threads_number; ++i) {
    threads.emplace_back(InitializeHashTables, std::ref(hash_tables), std::ref(thumbnails), threads_number, i);
  }
  for (int i = 0; i < threads_number; ++i) {
    threads[i].join();
  }*/
  for (int i = 0; i < hash_tables.size(); ++i) {
    hash_tables[i].insert(thumbnails.begin(), thumbnails.end());  
  }
  cout << timer.seconds_elapsed() << "\n";





  while(true) {
    cout << "enter image path:\n";
    string request;
    cin >> request;
    cout << "enter threshold:\n";
    cin >> kRange;

    VectorizedThumbnail request_thumb;

    try {
      request_thumb = GetThumbnailFromImage(request);
    } catch (cv::Exception) {
      cout << "wrong filename...re-enter please\n";
      continue;
    }

    unordered_set<string> unique_matches;
    map<double, string> result;

    for (size_t curr_table = 0; curr_table < kHashTableNumber; ++curr_table) {

      size_t bucket_number = hash_tables[curr_table].bucket(request_thumb);

      for (auto it = hash_tables[curr_table].begin(bucket_number); it != hash_tables[curr_table].end(bucket_number); ++it) {
        double cos_btw_vectors = std::abs(GetCosBtwVectors(*it, request_thumb));
        if (cos_btw_vectors >= kRange * kPrecision) {

          cout << cos_btw_vectors << "  " << it->get_source_path() << "\n";
          string match(it->get_source_path());
          if (unique_matches.find(match) == unique_matches.end()) {
            unique_matches.insert(it->get_source_path());
            result.insert(make_pair(cos_btw_vectors, it->get_source_path()));
          }
        }
      }
    }

    int matches_number = 0;
    for (auto it = result.rbegin(); it != result.rend(); ++it) {
      cv::Mat image = cv::imread(it->second);
      imwrite(result_path + to_string(matches_number) + ".jpg", image);
      cv::Mat compressed_image;
      cv::resize(image, compressed_image, cv::Size(kDefaultHeight, kDefaultWidth), 0, 0);
      cv::cvtColor(compressed_image, compressed_image, CV_BGR2GRAY);
      imwrite(result_path + "_compressed_" + to_string(matches_number) + ".jpg", compressed_image);
      ++matches_number;
    }
  }

  return 0;
}
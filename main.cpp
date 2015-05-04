#include <boost/filesystem.hpp>

#include <algorithm>
#include <iostream>
#include <vector>
#include <ctime>
#include <unordered_set>

#include "thumbnail.h"
#include "hash_function.h"
#include "perception_hash.h"
#include "dct_hash.h"

using namespace boost::filesystem;

using namespace std;

const string images_base = "/home/mikhail/image_search/pictures_base/test/";

//const string compressed_images = "/home/mikhail/image_search/compressed_images/";

Thumbnail CompressImage(std::string* from_path) {
    cv::Mat input_image = cv::imread(from_path->c_str());
    cv::Mat compressed_image;
    cv::resize(input_image, compressed_image, cv::Size(kDefaultHeight, kDefaultWidth), 0, 0);
    return Thumbnail(compressed_image, from_path->c_str());
}

void LoadImages(const string input_path, vector<Thumbnail>& loaded_images) {
    try {
        if (exists(input_path)) {

            if (is_directory(input_path)) {
                
                for (auto it = recursive_directory_iterator(input_path); it != recursive_directory_iterator(); ++it) {
                    if (is_regular_file(it->path())) {
                        cout << it->path() << "\n";
                        loaded_images.push_back(CompressImage(new string(it->path().string())));
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

    size_t input_threshold;
    cout << "enter threshold : ";
    cin >> input_threshold;
    if (input_threshold >= 0 && input_threshold <= kDefaultHeight * kDefaultWidth) {
        threshold = input_threshold;
    } else {
        threshold = 50;
    }
	path input_path(images_base);
	
    vector<Thumbnail> thumbnails;
    vector<Thumbnail> gray_thumbnails;

    LoadImages(images_base, thumbnails);
    

    std::vector<bitset<kDefaultHeight * kDefaultWidth>> hashes(thumbnails.size());
    for (size_t i = 0; i < thumbnails.size(); ++i) {
        hashes[i] = GetPerceptionHash(thumbnails[i]);
    }

    class MatchPair {
        public:
            MatchPair() :
                hamming_distance(0),
                first(0),
                second(0) {}
            MatchPair(size_t ham, size_t f, size_t s) :
                hamming_distance(ham),
                first(f),
                second(s){}

            bool operator < (const MatchPair& other) const {
                return hamming_distance < other.hamming_distance;
            }

            size_t hamming_distance;
            size_t first;
            size_t second;
    };


    vector<MatchPair> hamming_distances(thumbnails.size() * (thumbnails.size() - 1) / 2);
    size_t curr_pos = 0;
    for (size_t i = 0; i < thumbnails.size(); ++i) {
        for (size_t j = i + 1; j < thumbnails.size(); ++j) {
            hamming_distances[curr_pos++] = MatchPair(GetHammingDistance(hashes[i], hashes[j]), i, j);
        }
    }
    std::sort(hamming_distances.begin(), hamming_distances.end());
    for (MatchPair one_pair : hamming_distances) {
        if (one_pair.hamming_distance > 50) {
            break;
        }
        cout << one_pair.hamming_distance << "\n" << thumbnails[one_pair.first].GetSourceImagePath() << "\n" <<
                                                     thumbnails[one_pair.second].GetSourceImagePath() << "\n\n\n";               
    }
    return 0;
}
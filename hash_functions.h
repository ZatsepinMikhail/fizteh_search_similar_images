#pragma once

#include "constants.h"

#include <bitset>
#include <random>
#include <stdexcept>

class HashFunction {
public:
  HashFunction() 
  : random_vector_(kDefaultDimensionality) {
    static std::default_random_engine generator;
    static std::uniform_int_distribution<int> distribution(kRangeLeft, kRangeRight);
    for (size_t i = 0; i < kDefaultDimensionality; ++i) {
      random_vector_[i] = distribution(generator);
    }
  }

  //true if scalar product > 0, false otherwise.
  //rename factor_vector
  bool operator () (const std::vector<char>& factor_vector) const {
    if (random_vector_.size() != factor_vector.size()) {
      throw std::runtime_error("different vector sizes in scalar product");
    }
    double result;
    for (size_t curr_index = 0; curr_index < random_vector_.size(); ++curr_index) {
      result += random_vector_[curr_index] * factor_vector[curr_index];
    }
    return result > 0.;
  }

private:
  std::vector<char> random_vector_;
};


using compound_hash = std::bitset<kHashLength>;

class CompoundHashFunction {

public:
  CompoundHashFunction() {
    for (size_t i = 0; i < kHashLength; ++i) {
      hash_functions_.emplace_back();
    }
  }

size_t operator() (const VectorizedThumbnail& thumb) const {
  const std::vector<char>& factor_vector = thumb.vectorized_image_;
  compound_hash result;
  for (size_t curr_index = 0; curr_index < hash_functions_.size(); ++curr_index) {
    result[curr_index] = hash_functions_[curr_index](factor_vector);
  }
  return static_cast<size_t>(result.to_ulong());
}
private:
  std::vector<HashFunction> hash_functions_;
};
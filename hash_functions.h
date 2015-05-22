#pragma once

#include "constants.h"

#include <bitset>
#include <random>
#include <stdexcept>

void make_hyperplane_good(std::vector<short>& random_vector_) {
  int bit_sum = 0;
  for (short element : random_vector_) {
    bit_sum += (element > 0) ? 1 : 0;
  }
  
  if (bit_sum == 0 || bit_sum == (random_vector_.size() - 1)) {
    int random_index = random_vector_[0] % random_vector_.size();
    random_vector_[random_index] = -1 * random_vector_[random_index];
  }
}

class HashFunction {
public:
  HashFunction() 
  : random_vector_(kDefaultDimensionality) {
    static std::default_random_engine generator;
    static std::uniform_int_distribution<int> distribution(kRangeLeft, kRangeRight);

    //in II or III instead of I or IV
    for (size_t i = 0; i < kDefaultDimensionality; ++i) {
      random_vector_[i] = distribution(generator);
    }
    make_hyperplane_good(random_vector_);
  }

  //true if scalar product > 0, false otherwise.
  //rename factor_vector
  bool operator () (const std::vector<short>& factor_vector) const {
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
  std::vector<short> random_vector_;
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
  const std::vector<short>& factor_vector = thumb.vectorized_image_;
  compound_hash result;
  for (size_t curr_index = 0; curr_index < hash_functions_.size(); ++curr_index) {
    result[curr_index] = hash_functions_[curr_index](factor_vector);
  }
  return static_cast<size_t>(result.to_ulong());
}
private:
  std::vector<HashFunction> hash_functions_;
};

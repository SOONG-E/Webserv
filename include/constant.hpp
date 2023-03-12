#ifndef CONSTANT_HPP_
#define CONSTANT_HPP_

#include <string>

enum ErrorIndex {
  kPrefix,
  kArg,
  kOpen,
};

const int kNumOfErrors = 3;

const std::string kErrors[kNumOfErrors] = {"Error: ", "Invalid argument",
                                           "File open failed"};

#endif

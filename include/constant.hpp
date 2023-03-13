#ifndef CONSTANT_HPP_
#define CONSTANT_HPP_

#include <string>

enum ErrorIndex {
  kPrefix,
  kArg,
  kOpen,
  kToken,
};

const int kNumOfErrors = 4;

const std::string kErrors[kNumOfErrors] = {
    "Error: ", "Invalid argument", "File open failed", "Unexpected token"};

#endif

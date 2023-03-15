#ifndef CONSTANT_HPP_
#define CONSTANT_HPP_

#include <string>

enum DefaultIndex {
  kHost,
  kClientMaxBodySize,
};
enum ErrorIndex {
  kPrefix,
  kArg,
  kOpen,
  kToken,
};

const int kNumOfDefaults = 2;
const int kNumOfErrors = 4;

const std::string kDefaults[kNumOfDefaults] = {"0.0.0.0", "1m"};
const std::string kErrors[kNumOfErrors] = {
    "Error: ", "Invalid argument", "File open failed", "Unexpected token"};

#endif

#ifndef CONSTANT_HPP_
#define CONSTANT_HPP_

#include <string>

enum DefaultIndex {
  kHost,
  kPort,
  kClientMaxBodySize,
};
enum ErrorIndex {
  kPrefix,
  kArg,
  kOpen,
  kToken,
};

const int kNumOfDefaults = 3;
const int kNumOfErrors = 4;

const std::string kBase10 = "0123456789";
const std::string kDefaults[kNumOfDefaults] = {"0.0.0.0", "80", "1m"};
const std::string kErrors[kNumOfErrors] = {
    "Error: ", "Invalid argument", "File open failed", "Unexpected token"};

#endif

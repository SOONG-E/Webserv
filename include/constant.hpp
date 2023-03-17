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

const std::string kBase10 = "0123456789";
const std::string kDefaults[] = {"0.0.0.0", "80", "1m"};
const std::string kErrors[] = {"Error: ", "Invalid argument",
                               "File open failed", "Unexpected token"};

const std::string methods[8] = {"OPTIONS", "GET",    "HEAD",  "POST",
                                "PUT",     "DELETE", "TRACE", "CONNECT"};

#endif

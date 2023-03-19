#ifndef CONSTANT_HPP_
#define CONSTANT_HPP_

#include <string>

enum DefaultIndex {
  kHost,
  kPort,
  kClientMaxBodySize,
  kErrorPage,
};
enum ErrorIndex {
  kPrefix,
  kArg,
  kOpen,
  kToken,
};

const int METHODS_COUNT = 3;
const std::size_t HEADER_MAX_SIZE = 8192;
const std::string kBase10 = "0123456789";
const std::string kWhitespace = " \t\n\v\f\r";
const std::string CRLF = "\r\n";
const std::string DOUBLE_CRLF = "\r\n\r\n";
const std::string kDefaults[] = {"0.0.0.0", "80", "1m", "page/error.html"};
const std::string kErrors[] = {"Error: ", "Invalid argument",
                               "File open failed", "Unexpected token"};
const std::string METHODS[] = {"GET", "POST", "DELETE"};

#endif

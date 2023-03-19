#ifndef CONSTANT_HPP_
#define CONSTANT_HPP_

#include <string>

enum DefaultIndex {
  HOST,
  PORT,
  CLIENT_MAX_BODY_SIZE,
  ERROR_PAGE,
};

enum ErrorIndex {
  PREFIX,
  ARG,
  OPEN,
  TOKEN,
};

const int METHODS_COUNT = 3;
const std::size_t HEADER_MAX_SIZE = 8192;

const std::string BASE10 = "0123456789";
const std::string CRLF = "\r\n";
const std::string DOUBLE_CRLF = "\r\n\r\n";
const std::string WHITESPACE = " \t\n\v\f\r";
const std::string DEFAULTS[] = {"0.0.0.0", "80", "1m", "page/error.html"};
const std::string ERRORS[] = {"Error: ", "Invalid argument", "File open failed",
                              "Unexpected token"};
const std::string METHODS[] = {"GET", "POST", "DELETE"};

#endif

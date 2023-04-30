#ifndef CONSTANT_HPP_
#define CONSTANT_HPP_

#include <ctime>
#include <map>
#include <set>

#include "setting.hpp"

enum MethodIndex {
  GET,
  POST,
  DELETE,
  HEAD,
  OPTIONS,
  PUT,
  TRACE,
  CONNECT,
};

const std::string EMPTY_STRING = "";
const std::string SESSION_ID_FIELD = "Session-ID";
const int DEFAULT_FD = -1;
const int METHODS_COUNT = 8;
const std::size_t NPOS = -1;

const std::string BASE10 = "0123456789";
const std::string CRLF = "\r\n";
const std::string DOUBLE_CRLF = "\r\n\r\n";
const std::string LF = "\n";
const std::string DOUBLE_LF = "\n\n";
const std::string WHITESPACE = " \t\n\v\f\r";
const std::string METHODS[] = {
    "GET", "POST", "DELETE", "HEAD", "OPTIONS", "PUT", "TRACE", "CONNECT",
};

template <typename T>
T ERROR() {
  return -1;
}

#endif

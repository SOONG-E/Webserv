#ifndef CONSTANT_HPP_
#define CONSTANT_HPP_

#include <string>

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

const std::size_t BUF_SIZE = 65536;
const std::size_t HEADER_MAX_SIZE = 8192;
const int METHODS_COUNT = 8;
const std::size_t NPOS = -1;

const std::string BASE10 = "0123456789";
const std::string CRLF = "\r\n";
const std::string DEFAULT_PATH = "conf/tester.conf";
const std::string DEFAULT_PORT = "80";
const std::string DOUBLE_CRLF = "\r\n\r\n";
const std::string WHITESPACE = " \t\n\v\f\r";
const std::string METHODS[] = {
    "GET", "POST", "DELETE", "HEAD", "OPTIONS", "PUT", "TRACE", "CONNECT",
};
const time_t KEEPALIVE_TIMEOUT = 30;

template <typename T>
T ERROR() {
  return -1;
}

#endif

#ifndef CONSTANT_HPP_
#define CONSTANT_HPP_

#include <string>

enum ErrorIndex {
  PREFIX,
  ARG,
  OPEN,
  TOKEN,
};

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

const int METHODS_COUNT = 8;
const std::size_t BUF_SIZE = 65536;
const std::size_t HEADER_MAX_SIZE = 8192;

const std::string BASE10 = "0123456789";
const std::string CRLF = "\r\n";
const std::string DEFAULT_PATH = "conf/tester.conf";
const std::string DOUBLE_CRLF = "\r\n\r\n";
const std::string WHITESPACE = " \t\n\v\f\r";
const std::string ERRORS[] = {"Error: ", "Invalid argument", "File open failed",
                              "Unexpected token"};
const std::string METHODS[] = {"GET",     "POST", "DELETE", "HEAD",
                               "OPTIONS", "PUT",  "TRACE",  "CONNECT"};

#endif

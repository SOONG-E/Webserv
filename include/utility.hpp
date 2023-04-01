#ifndef UTILITY_HPP_
#define UTILITY_HPP_

#include <unistd.h>

#include <ctime>
#include <sstream>
#include <string>
#include <vector>

#include "ResponseStatus.hpp"
#include "constant.hpp"

std::string formatTime(const char* format,
                       std::time_t timestamp = std::time(NULL));
std::size_t hexToInt(const std::string& value);
bool isDirectory(const std::string& path);
bool isNumber(const std::string& str);
std::string readFile(const std::string& filename);
std::vector<std::string> split(const std::string& content,
                               const std::string& delim = WHITESPACE);
std::size_t stoi(const std::string& value);
std::string trim(const std::string& str);
std::string getAbsolutePath(const std::string& uri);

template <typename T>
std::string toString(T value) {
  std::ostringstream oss;
  oss << value;
  return oss.str();
}

#endif

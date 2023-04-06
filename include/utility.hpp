#ifndef UTILITY_HPP_
#define UTILITY_HPP_

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
void removeDirectory(const std::string& path);
std::vector<std::string> split(const std::string& content,
                               const std::string& delim = WHITESPACE);
std::size_t stoi(const std::string& value);
std::string trim(const std::string& str);

template <typename T>
std::string join(const T& strings, const std::string& delim) {
  std::string joined;
  for (typename T::const_iterator str_iter = strings.begin();
       str_iter != strings.end(); ++str_iter) {
    if (str_iter != strings.begin()) {
      joined += delim;
    }
    joined += *str_iter;
  }
  return joined;
}

template <typename T>
std::string toString(T value) {
  std::ostringstream oss;
  oss << value;
  return oss.str();
}

#endif

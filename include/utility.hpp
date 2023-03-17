#ifndef UTILITY_HPP_
#define UTILITY_HPP_

#include <sstream>
#include <string>
#include <vector>

bool isNumber(const std::string& str);
std::string readFile(const char* filename);
std::vector<std::string> split(const std::string& content,
                               const std::string& delim = " \t\n\v\f\r");
std::size_t stoi(const std::string& value);
std::size_t hexToInt(const std::string& value);

template <typename T>
std::string toString(T value) {
  std::ostringstream oss;
  oss << value;
  return oss.str();
}

#endif

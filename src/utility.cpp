#include "utility.hpp"

#include <fstream>

#include "constant.hpp"
#include "exception.hpp"

std::string trim(std::string str) {
  std::size_t start = str.find_first_not_of(WHITESPACE);
  std::size_t end = str.find_last_not_of(WHITESPACE);

  return (str.substr(start, end + 1));
}

bool isNumber(const std::string& str) {
  return str.find_first_not_of(BASE10) == std::string::npos;
}

std::string readFile(const char* filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    throw FileOpenException();
  }
  std::ostringstream content;
  content << file.rdbuf();
  file.close();
  return content.str();
}

std::string readFile(const std::string& filename) {
  return readFile(filename.c_str());
}

std::vector<std::string> split(const std::string& content,
                               const std::string& delim) {
  std::vector<std::string> substrings;
  std::size_t start = 0;
  std::size_t end;
  while (true) {
    end = content.find_first_of(delim, start);
    if (end == std::string::npos) break;
    substrings.push_back(content.substr(start, end - start));
    start = end + 1;
  }
  substrings.push_back(content.substr(start));
  return substrings;
}

std::size_t stoi(const std::string& value) {
  std::istringstream iss(value);
  std::size_t num;
  iss >> num;
  if (iss.fail()) {
    // throw;
  }
  return num;
}

std::size_t hexToInt(const std::string& value) {
  std::size_t out;
  std::istringstream iss(value);
  iss >> std::hex >> out;
  return out;
}

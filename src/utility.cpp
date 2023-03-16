#include "utility.hpp"

#include <fstream>
#include <sstream>

#include "constant.hpp"
#include "exception.hpp"

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

int stoi(const std::string& str) {
  std::istringstream iss(str);
  int num;
  iss >> num;
  if (iss.fail()) {
    throw ConfigException(kErrors[kToken]);
  }
  return num;
}

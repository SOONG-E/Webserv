#include "utility.hpp"

#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <fstream>

#include "exception.hpp"

std::string formatTime(const char* format, std::time_t timestamp) {
  char buf[80];
  std::strftime(buf, sizeof(buf), format, std::localtime(&timestamp));
  return buf;
}

std::size_t hexToInt(const std::string& value) {
  std::size_t out;
  std::istringstream iss(value);
  iss >> std::hex >> out;
  return out;
}

bool isDirectory(const std::string& path) {
  struct stat statbuf;
  if (stat(path.c_str(), &statbuf) == ERROR<int>()) {
    return false;
  }
  return S_ISDIR(statbuf.st_mode);
}

bool isNumber(const std::string& str) {
  return str.find_first_not_of(BASE10) == std::string::npos;
}

std::string readFile(const std::string& filename) {
  std::ifstream file(filename.c_str());
  if (!file.is_open()) {
    throw FileOpenException();
  }
  std::ostringstream content;
  content << file.rdbuf();
  file.close();
  return content.str();
}

int removeDirectory(const std::string& path) {
  DIR* dir = opendir(path.c_str());
  if (!dir) {
    return ERROR<int>();
  }
  int error = 0;
  struct dirent* entry;
  while (!error && (entry = readdir(dir))) {
    std::string name = entry->d_name;
    if (name == "." || name == "..") continue;
    std::string subpath = (*path.rbegin() == '/') ? path : path + '/';
    subpath += name;
    if (isDirectory(subpath)) {
      error = removeDirectory(subpath);
    } else {
      error = unlink(subpath.c_str());
    }
  }
  closedir(dir);
  if (!error) {
    error = rmdir(path.c_str());
  }
  return error;
}

std::vector<std::string> split(const std::string& content,
                               const std::string& delim) {
  std::vector<std::string> substrings;
  std::size_t start = 0;
  std::size_t end;
  while (start != content.size()) {
    end = content.find_first_of(delim, start);
    if (end == std::string::npos) {
      substrings.push_back(content.substr(start));
      break;
    }
    substrings.push_back(content.substr(start, end - start));
    start = end + 1;
  }
  return substrings;
}

std::set<std::string> splitToSet(const std::string& content,
                                 const std::string& delim) {
  std::set<std::string> substrings;
  std::size_t start = 0;
  std::size_t end;
  while (start != content.size()) {
    end = content.find_first_of(delim, start);
    if (end == std::string::npos) {
      substrings.insert(content.substr(start));
      break;
    }
    substrings.insert(content.substr(start, end - start));
    start = end + 1;
  }
  return substrings;
}

std::size_t stoi(const std::string& value) {
  std::istringstream iss(value);
  std::size_t num;
  iss >> num;
  if (iss.fail()) {
    throw std::invalid_argument("stoi() failed: " + value);
  }
  return num;
}

std::string trim(const std::string& str) {
  std::size_t start = str.find_first_not_of(WHITESPACE);
  std::size_t end = str.find_last_not_of(WHITESPACE);
  if (start == std::string::npos) return "";
  return (str.substr(start, end - start + 1));
}

std::string getIpFromKey(const std::string key) {
  std::size_t boundary = key.find(":");
  return key.substr(0, boundary);
}

std::string getPortFromKey(const std::string key) {
  std::size_t boundary = key.find(":");
  return key.substr(boundary + 1);
}
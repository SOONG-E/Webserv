#ifndef FILE_HPP_
#define FILE_HPP_

#include <sys/types.h>

#include <ctime>
#include <string>

struct File {
  friend bool operator<(const File& lhs, const File& rhs) {
    return lhs.name < rhs.name;
  }

  std::string name;
  std::time_t last_modified;
  off_t size;
};

#endif

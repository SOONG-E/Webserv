#ifndef LOCATION_HPP_
#define LOCATION_HPP_

#include <set>
#include <string>

struct Location {
  std::string uri;
  std::set<std::string> allowed_methods;
  std::set<std::string> try_files;
};

#endif

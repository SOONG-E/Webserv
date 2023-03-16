#ifndef LOCATION_HPP_
#define LOCATION_HPP_

#include <set>
#include <string>

struct Location {
  std::string uri;
  std::set<std::string> allowed_methods;
  std::string return_code;
  std::string return_url;
  std::string root;
  std::string autoindex;
  std::set<std::string> index;
  std::set<std::string> try_files;
};

#endif

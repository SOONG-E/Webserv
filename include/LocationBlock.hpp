#ifndef LOCATION_BLOCK_HPP_
#define LOCATION_BLOCK_HPP_

#include <set>
#include <string>

struct LocationBlock {
  enum Index {
    ROOT,
    AUTOINDEX,
    INDEX,
  };

  static const std::string DEFAULTS[];

  LocationBlock();

  std::string uri;
  std::set<std::string> allowed_methods;
  std::string return_code;
  std::string return_url;
  std::string root;
  std::string autoindex;
  std::set<std::string> index;
};

#endif

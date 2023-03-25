#ifndef LOCATION_BLOCK_HPP_
#define LOCATION_BLOCK_HPP_

#include <set>
#include <string>

struct LocationBlock {
  enum Index {
    CLIENT_MAX_BODY_SIZE,
    ROOT,
    AUTOINDEX,
    INDEX,
  };

  static const std::string DEFAULTS[];

  LocationBlock();

  void setBodyLimit(const std::string& raw);

  std::string uri;
  std::size_t body_limit;
  std::set<std::string> allowed_methods;
  std::string return_code;
  std::string return_url;
  std::string root;
  std::string autoindex;
  std::set<std::string> index;
};

#endif

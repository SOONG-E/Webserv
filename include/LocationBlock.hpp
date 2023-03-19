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

  LocationBlock() : root(DEFAULTS[ROOT]), autoindex(DEFAULTS[AUTOINDEX]) {
    for (int i = 0; i < METHODS_COUNT; ++i) {
      allowed_methods.insert(METHODS[i]);
    }
    index.insert(DEFAULTS[INDEX]);
  }

  std::string uri;
  std::set<std::string> allowed_methods;
  std::string return_code;
  std::string return_url;
  std::string root;
  std::string autoindex;
  std::set<std::string> index;
  // std::set<std::string> try_files;
};

const std::string LocationBlock::DEFAULTS[] = {"html", "off", "index.html"};

#endif

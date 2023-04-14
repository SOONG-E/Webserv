#ifndef SERVER_BLOCK_HPP_
#define SERVER_BLOCK_HPP_

#include <map>
#include <set>
#include <string>
#include <vector>

#include "Listen.hpp"
#include "Location.hpp"

struct ServerBlock {
 public:
  std::vector<Listen> listens;
  std::set<std::string> server_names;
  std::map<std::string, std::string> error_pages;
  std::vector<Location> locations;
};

#endif

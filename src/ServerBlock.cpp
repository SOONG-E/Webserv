#include "ServerBlock.hpp"

#include <iostream>

#include "constant.hpp"

ServerBlock::ServerBlock() : body_limit_(kDefaults[kClientMaxBodySize]) {}

ServerBlock::ServerBlock(const ServerBlock& origin)
    : listens_(origin.listens_),
      server_names_(origin.server_names_),
      error_pages_(origin.error_pages_),
      body_limit_(origin.body_limit_),
      location_blocks_(origin.location_blocks_) {}

ServerBlock& ServerBlock::operator=(const ServerBlock& origin) {
  if (this != &origin) {
    listens_ = origin.listens_;
    server_names_ = origin.server_names_;
    error_pages_ = origin.error_pages_;
    body_limit_ = origin.body_limit_;
    location_blocks_ = origin.location_blocks_;
  }
  return *this;
}

ServerBlock::~ServerBlock() {}

std::vector<Listen>& ServerBlock::getListens(void) { return listens_; }

void ServerBlock::setBodyLimit(const std::string& body_limit) {
  body_limit_ = body_limit;
}

void ServerBlock::addListen(const std::string& raw) {
  listens_.push_back(Listen(raw));
}

void ServerBlock::addServerName(const std::string& name) {
  server_names_.insert(name);
}

void ServerBlock::addErrorPage(const std::string& code,
                               const std::string& page) {
  error_pages_.insert(std::make_pair(code, page));
}

void ServerBlock::addLocationBlock(const LocationBlock& location_block) {
  location_blocks_.push_back(location_block);
}

std::set<std::string> ServerBlock::keys(void) const {
  std::set<std::string> keys;
  for (int i = 0; i < listens_.size(); ++i) {
    for (std::set<std::string>::const_iterator names_iter =
             server_names_.begin();
         names_iter != server_names_.end(); ++names_iter) {
      keys.insert(listens_[i].raw + ":" + *names_iter);
    }
  }
  return keys;
}

void ServerBlock::print(const int index) const {
  std::cout << "[ server block " << index << " ]\n";
  for (int i = 0; i < listens_.size(); ++i) {
    std::cout << "listen: " << listens_[i].host << ":" << listens_[i].port
              << "\n";
  }
  std::cout << "server name: ";
  for (std::set<std::string>::const_iterator it = server_names_.begin();
       it != server_names_.end(); ++it) {
    std::cout << *it << " ";
  }
  std::cout << "\n";
  for (std::map<std::string, std::string>::const_iterator it =
           error_pages_.begin();
       it != error_pages_.end(); ++it) {
    std::cout << "error page: " << it->first << " " << it->second << "\n";
  }
  std::cout << "client max body size: " << body_limit_ << "\n";
}

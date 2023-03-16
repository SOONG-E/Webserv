#include "Server.hpp"

#include <iostream>

#include "constant.hpp"

Server::Server() : body_limit_(kDefaults[kClientMaxBodySize]) {}

Server::Server(const Server& origin) {}

Server& Server::operator=(const Server& origin) { return *this; }

Server::~Server() {}

std::vector<Listen*>& Server::getListens(void) { return listens_; }

void Server::setBodyLimit(const std::string& body_limit) {
  body_limit_ = body_limit;
}

void Server::addListen(const std::string& raw) {
  listens_.push_back(new Listen(raw));
}

void Server::addServerName(const std::string& name) {
  server_names_.insert(name);
}

void Server::addErrorPage(const std::string& code, const std::string& page) {
  error_pages_.insert(std::make_pair(code, page));
}

void Server::addLocation(Location* location) { locations_.push_back(location); }

std::set<std::string> Server::keys(void) const {
  std::set<std::string> keys;
  for (int i = 0; i < listens_.size(); ++i) {
    for (std::set<std::string>::const_iterator names_iter =
             server_names_.begin();
         names_iter != server_names_.end(); ++names_iter) {
      keys.insert(listens_[i]->raw + ":" + *names_iter);
    }
  }
  return keys;
}

void Server::print(const int index) const {
  std::cout << "[ server block " << index << " ]\n";
  for (int i = 0; i < listens_.size(); ++i) {
    std::cout << "listen: " << listens_[i]->host << ":" << listens_[i]->port
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

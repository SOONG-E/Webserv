#include "Server.hpp"

#include "ConfigException.hpp"

Server::Server() {}

Server::Server(const Server& origin) {}

Server& Server::operator=(const Server& origin) {}

Server::~Server() {}

const std::set<std::string>& Server::getServerNames(void) const {
  return server_names_;
}

void Server::setBodyLimit(const int body_limit) { body_limit_ = body_limit; }

void Server::addListen(const std::string& host, const int port) {
  listens_.insert(std::make_pair(host, port));
}

void Server::addServerName(const std::string& name) {
  if (server_names_.find(name) != server_names_.end()) {
    throw ConfigException(kErrors[kToken]);
  }
  server_names_.insert(name);
}

void Server::addErrorPage(const int code, const std::string& page) {
  error_pages_.insert(std::make_pair(code, page));
}

void Server::addLocation(const Location& location) {
  locations_.push_back(location);
}

#include "Server.hpp"

#include "constant.hpp"
#include "exception.hpp"
#include "utility.hpp"

Server::Server() {}

Server::Server(const Server& origin) {}

Server& Server::operator=(const Server& origin) {}

Server::~Server() {}

void Server::setBodyLimit(const std::string& body_limit) {
  body_limit_ = body_limit;
}

void Server::addListen(const std::string& listen) {
  listens_.push_back(Server::Listen(listen));
}

void Server::addServerName(const std::string& name) {
  server_names_.insert(name);
}

void Server::addErrorPage(const int code, const std::string& page) {
  error_pages_.insert(std::make_pair(code, page));
}

void Server::addLocation(const Location& location) {
  locations_.push_back(location);
}

void Server::init(void) {
  listens_.clear();
  server_names_.clear();
  error_pages_.clear();
  body_limit_ = kDefaults[kClientMaxBodySize];
  locations_.clear();
}

std::set<std::string> Server::keys(void) const {
  std::set<std::string> keys;
  std::vector<Listen>::iterator listens_iter = listens_.begin();
  std::vector<Listen>::iterator listens_end = listens_.end();
  std::set<std::string>::iterator names_iter;
  std::set<std::string>::iterator names_end = server_names_.end();
  while (listens_iter != listens_end) {
    names_iter = server_names_.begin();
    while (names_iter != names_end) {
      keys.insert(*listens_iter + ":" + *names_iter);
      ++names_iter;
    }
    ++listens_iter;
  }
  return keys;
}

Server::Listen::Listen(const std::string& raw)
    : listen(raw), host(kDefaults[kHost]) {
  if (raw == "") return;
  std::vector<std::string> splitted = split(raw, ":");
  if (splitted.size() == 1) {
    port = stoi(splitted[0]);
  } else if (splitted.size() == 2) {
    host = listen[0];
    port = stoi(splitted[1]);
  } else {
    throw ConfigException(kErrors[kToken]);
  }
}

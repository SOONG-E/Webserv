#include "Config.hpp"

Config::Config() {}

Config::Config(const Config& origin) {}

Config& Config::operator=(const Config& origin) {}

Config::~Config() {}

void Config::addServer(const Server& server) {
  std::set<std::string>::iterator iter = server.getServerNames().begin();
  std::set<std::string>::iterator end = server.getServerNames().end();
  while (iter != end) {
    servers_.insert(std::make_pair(*iter, server));
    ++iter;
  }
}

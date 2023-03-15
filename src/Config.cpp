#include "Config.hpp"

Config::Config() {}

Config::Config(const Config& origin) {}

Config& Config::operator=(const Config& origin) {}

Config::~Config() {}

const std::vector<Server>& Config::getServers(void) const { return servers_; }

void Config::addServer(const Server& server) {
  servers_.push_back(server);
  std::set<std::string> keys = server.keys();
  std::set<std::string>::iterator keys_iter = keys.begin();
  std::set<std::string>::iterator keys_end = keys.end();
  while (keys_iter != keys_end) {
    servers_table_.insert(std::make_pair(*keys_iter, server));
    ++keys_iter;
  }
}

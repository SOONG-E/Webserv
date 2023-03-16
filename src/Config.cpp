#include "Config.hpp"

Config::Config() {}

Config::Config(const Config& origin) {}

Config& Config::operator=(const Config& origin) { return *this; }

Config::~Config() {}

std::vector<Server*>& Config::getServers(void) { return servers_; }

void Config::addServer(Server* server) {
  servers_.push_back(server);
  std::set<std::string> keys = server->keys();
  for (std::set<std::string>::const_iterator keys_iter = keys.begin();
       keys_iter != keys.end(); ++keys_iter) {
    servers_table_.insert(std::make_pair(*keys_iter, server));
  }
}

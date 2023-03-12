#include "Config.hpp"

Config::Config() {}
Config::Config(const Config& origin) {}
Config::~Config() {}

Config& Config::operator=(const Config& origin) {}

void Config::insertServer(const std::string server_name, const Server server) {
  servers.insert(std::pair<std::string, Server>(server_name, server));
}

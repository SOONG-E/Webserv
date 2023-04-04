#include "Config.hpp"

#include <cstdlib>

#include "Error.hpp"

Config::Config() {}

Config::Config(const Config& origin) : server_blocks_(origin.server_blocks_) {}

Config& Config::operator=(const Config& origin) {
  if (this != &origin) {
    server_blocks_ = origin.server_blocks_;
  }
  return *this;
}

Config::~Config() {}

const std::vector<ServerBlock>& Config::getServerBlocks(void) const {
  return server_blocks_;
}

void Config::addServerBlock(const ServerBlock& server_block) {
  validate(server_block);
  server_blocks_.push_back(server_block);
}

void Config::validate(const ServerBlock& server_block) const {
  static std::size_t total_count;
  static std::set<std::string> total_keys;
  std::set<std::string> each_keys = server_block.keys();
  total_count += each_keys.size();
  total_keys.insert(each_keys.begin(), each_keys.end());
  if (total_count != total_keys.size()) {
    Error::log(Error::INFO[ETOKEN], "", EXIT_FAILURE);
  }
}

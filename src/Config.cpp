#include "Config.hpp"

Config::Config() {}

Config::Config(const Config& origin)
    : server_blocks_(origin.server_blocks_),
      servers_table_(origin.servers_table_) {}

Config& Config::operator=(const Config& origin) {
  if (this != &origin) {
    server_blocks_ = origin.server_blocks_;
    servers_table_ = origin.servers_table_;
  }
  return *this;
}

Config::~Config() {}

std::vector<ServerBlock>& Config::getServerBlocks(void) {
  return server_blocks_;
}

void Config::addServerBlock(const ServerBlock& server_block) {
  server_blocks_.push_back(server_block);
  std::set<std::string> keys = server_block.keys();
  for (std::set<std::string>::const_iterator keys_iter = keys.begin();
       keys_iter != keys.end(); ++keys_iter) {
    servers_table_.insert(std::make_pair(*keys_iter, server_block));
  }
}

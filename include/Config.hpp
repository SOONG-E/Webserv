#ifndef CONFIG_HPP_
#define CONFIG_HPP_

#include "ServerBlock.hpp"

class Config {
 public:
  Config();
  Config(const Config& origin);
  Config& operator=(const Config& origin);
  virtual ~Config();

  std::vector<ServerBlock>& getServerBlocks(void);

  void addServerBlock(const ServerBlock& server_block);

 private:
  std::vector<ServerBlock> server_blocks_;
  std::map<std::string, ServerBlock> servers_table_;
};

#endif

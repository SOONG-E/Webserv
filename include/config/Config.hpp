#ifndef CONFIG_HPP_
#define CONFIG_HPP_

#include <map>
#include <string>
#include <vector>

#include "ServerBlock.hpp"

class Config {
 public:
  Config();
  Config(const Config& origin);
  Config& operator=(const Config& origin);
  ~Config();

  const std::vector<ServerBlock>& getServerBlocks(void) const;

  void addServerBlock(const ServerBlock& server_block);

 private:
  void validate(const ServerBlock& server_block) const;

  std::vector<ServerBlock> server_blocks_;
};

#endif

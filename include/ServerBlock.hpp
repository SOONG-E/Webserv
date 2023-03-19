#ifndef SERVER_BLOCK_HPP_
#define SERVER_BLOCK_HPP_

#include <map>
#include <set>
#include <string>
#include <vector>

#include "Listen.hpp"
#include "LocationBlock.hpp"

class ServerBlock {
 public:
  ServerBlock();
  ServerBlock(const ServerBlock& origin);
  ServerBlock& operator=(const ServerBlock& origin);
  virtual ~ServerBlock();

  const std::vector<Listen>& getListens(void) const;
  const std::set<std::string>& getServerNames(void) const;
  const std::vector<LocationBlock>& getLocationBlocks(void) const;

  void addListen(const std::string& socket_key);
  void addServerName(const std::string& name);
  void addErrorPage(const std::string& code, const std::string& page);
  void setBodyLimit(const std::string& body_limit);
  void addLocationBlock(const LocationBlock& location_block);

  void reset(void);
  std::set<std::string> keys(void) const;
  void print(const int index) const;

 private:
  std::vector<Listen> listens_;
  std::set<std::string> server_names_;
  std::map<std::string, std::string> error_pages_;
  std::size_t body_limit_;
  std::vector<LocationBlock> location_blocks_;
};

#endif

#ifndef SERVER_BLOCK_HPP_
#define SERVER_BLOCK_HPP_

#include <map>
#include <set>
#include <string>
#include <vector>

#include "Listen.hpp"
#include "LocationBlock.hpp"

class ServerBlock {
  static const int DEFAULT_KEY;

 public:
  ServerBlock();
  ServerBlock(const ServerBlock& origin);
  ServerBlock& operator=(const ServerBlock& origin);
  ~ServerBlock();

  int getKey(void) const;
  const std::vector<Listen>& getListens(void) const;
  const std::set<std::string>& getServerNames(void) const;
  const std::string& getErrorPage(const std::string& code) const;
  const std::vector<LocationBlock>& getLocationBlocks(void) const;

  void setKey(const int key);
  void addListen(const std::string& server_socket_key);
  void addServerName(const std::string& name);
  void addErrorPage(const std::string& code, const std::string& page);
  void addLocationBlock(const LocationBlock& location_block);

  std::set<std::string> keys(void) const;
  const LocationBlock& findLocationBlock(const std::string& request_uri) const;
  void setDefault(void);
  void clear(void);

 private:
  std::size_t find(const std::string& uri) const;
  const LocationBlock& redirect(const LocationBlock& location) const;

  int key_;
  std::vector<Listen> listens_;
  std::set<std::string> server_names_;
  std::map<std::string, std::string> error_pages_;
  std::vector<LocationBlock> location_blocks_;
};

#endif

#ifndef CONFIG_HPP_
#define CONFIG_HPP_

#include "Server.hpp"

class Config {
 public:
  Config();
  Config(const Config& origin);
  Config& operator=(const Config& origin);
  virtual ~Config();

  const std::vector<Server>& getServers(void) const;

  void addServer(const Server& server);

 private:
  std::vector<Server> servers_;
  std::map<std::string, Server> servers_table_;
};

#endif

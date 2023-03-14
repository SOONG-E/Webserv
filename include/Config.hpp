#ifndef CONFIG_HPP_
#define CONFIG_HPP_

#include "Server.hpp"

class Config {
 public:
  Config();
  Config(const Config& origin);
  Config& operator=(const Config& origin);

  virtual ~Config();

  void addServer(const Server& server);

 private:
  std::map<std::string, Server> servers_;
};

#endif

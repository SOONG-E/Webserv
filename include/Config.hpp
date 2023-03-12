#ifndef CONFIG_HPP_
#define CONFIG_HPP_

#include <map>
#include <string>

#include "Server.hpp"

class Config {
 public:
  Config();
  Config(const Config& origin);
  virtual ~Config();

  Config& operator=(const Config& origin);

  void insertServer(const std::string server_name, const Server server);

 private:
  std::map<std::string, Server> servers;
};

#endif

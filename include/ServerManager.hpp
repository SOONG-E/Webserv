#ifndef SERVER_MANAGER_HPP_
#define SERVER_MANAGER_HPP_

#include <map>
#include <string>

#include "Config.hpp"
#include "TcpServer.hpp"

class ServerManager {
 public:
  ServerManager(const Config &config);
  ~ServerManager();

 private:
  void registerServer(const Config &config);

  std::map<std::string, TcpServer> tcp_servers_;
};

#endif
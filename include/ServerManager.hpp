#ifndef SERVER_MANAGER_HPP_
#define SERVER_MANAGER_HPP_

#include <map>
#include <string>

#include "Config.hpp"
#include "TcpServer.hpp"
#include "utility.hpp"

class ServerManager {
 public:
  ServerManager(const Config &config);
  ~ServerManager();

 private:
  typedef std::map<std::string, TcpServer *> TcpServerType;

  void registerServer(const Config &config);
  TcpServer *seekTcpServer(std::string key);

  TcpServerType tcp_servers_;
};

#endif
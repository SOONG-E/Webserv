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
  typedef std::map<std::string, TcpServer *> TcpServerType;
  typedef std::vector<HttpServer *> HttpServerType;

  void registerServer(const Config &config);
  TcpServer *seekTcpServer(const std::string &key);
  TcpServer *createTcpServer(const std::string &key);
  HttpServer *createHttpServer(const ServerBlock &server_block);

  TcpServerType tcp_servers_;
  HttpServerType http_servers_;
};

#endif
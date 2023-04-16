#ifndef SERVER_MANAGER_HPP_
#define SERVER_MANAGER_HPP_

#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <exception>
#include <map>
#include <string>

#include "Config.hpp"
#include "TcpServer.hpp"

class ServerManager {
 public:
  ServerManager(const Config &config);
  ~ServerManager();

  void bindServers(void);

 private:
  typedef std::map<std::string, TcpServer *> TcpServerType;
  typedef std::vector<HttpServer *> HttpServerType;

  void registerServer(const Config &config);
  TcpServer *seekTcpServer(const std::string &key);
  TcpServer *createTcpServer(const std::string &key);
  HttpServer *createHttpServer(const ServerBlock &server_block);

  int createListenSocket(void) const;
  struct addrinfo *getAddrInfo(const std::string ip, const std::string port);

  TcpServerType tcp_servers_;
  HttpServerType http_servers_;
  std::set<int> listen_sockets_;
};

#endif
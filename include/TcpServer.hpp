#ifndef TCP_SERVER_HPP_
#define TCP_SERVER_HPP_

#include <map>
#include <string>

#include "HttpServer.hpp"
#include "ServerBlock.hpp"
#include "utility.hpp"

class TcpServer {
 public:
  typedef std::map<std::string, const HttpServer *> HttpServerType;

  TcpServer(const std::string &key);
  TcpServer(const std::string &ip, const std::string &port);
  TcpServer(const TcpServer &origin);
  ~TcpServer();

  std::string getIp(void) const;
  std::string getPort(void) const;
  HttpServer *getDefaultServer(void) const;
  HttpServerType getVirtualServers(void) const;

  void appendServer(const ServerBlock &servers,
                    const HttpServer *virtual_server);

 private:
  void setDefaultServer(HttpServer *default_server);

  const std::string ip_;
  const std::string port_;
  HttpServer *default_server_;
  HttpServerType virtual_servers_;
};

#endif
#ifndef TCP_SERVER_HPP_
#define TCP_SERVER_HPP_

#include <map>
#include <string>

#include "HttpServer.hpp"
#include "ServerBlock.hpp"
#include "utility.hpp"

class TcpServer {
 public:
  TcpServer(const std::string &key);
  TcpServer(const std::string &ip, const std::string &port);
  TcpServer(const TcpServer &origin);
  ~TcpServer();

  void appendServer(const ServerBlock &servers,
                    const HttpServer *virtual_server);

 private:
  const std::string ip_;
  const std::string port_;  // 타입 변경하기

  std::map<std::string, const HttpServer *> virtual_servers_;
};

#endif
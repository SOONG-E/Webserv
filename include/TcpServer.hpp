#ifndef TCP_SERVER_HPP_
#define TCP_SERVER_HPP_

#include <map>
#include <string>

#include "HttpServer.hpp"
#include "ServerBlock.hpp"

class TcpServer {
 public:
  TcpServer(const std::string ip, const std::string port,
            const HttpServer *virtual_server);
  ~TcpServer();

  void appendServer(const HttpServer *virtual_server);

 private:
  std::string ip_;
  std::string port_;  // 타입 변경하기

  std::map<std::string, HttpServer *> virtual_servers_;
};

#endif
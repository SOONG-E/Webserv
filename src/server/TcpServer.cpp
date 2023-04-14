#include "TcpServer.hpp"

TcpServer::TcpServer(const std::string ip, const std::string port,
                     const HttpServer *virtual_server)
    : ip_(ip), port_(port) {
  //
}

void TcpServer::appendServer(const HttpServer *virtual_server) {}

TcpServer::~TcpServer() {}
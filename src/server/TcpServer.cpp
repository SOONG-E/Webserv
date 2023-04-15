#include "TcpServer.hpp"

TcpServer::TcpServer(const std::string ip, const std::string port)
    : ip_(ip), port_(port) {}

void TcpServer::appendServer(const ServerBlock servers) {
  HttpServer *new_server = new HttpServer(servers);
  for (std::set<std::string>::iterator it = servers.server_names.begin();
       it != servers.server_names.end(); ++it) {
    virtual_servers_[*it] = new_server;
  }
}

TcpServer::~TcpServer() {}
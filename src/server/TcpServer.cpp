#include "TcpServer.hpp"

TcpServer::TcpServer(const std::string &key)
    : ip_(getIpFromKey(key)), port_(getPortFromKey(key)) {}

TcpServer::TcpServer(const std::string &ip, const std::string &port)
    : ip_(ip), port_(port) {}

TcpServer::TcpServer(const TcpServer &origin)
    : ip_(origin.ip_),
      port_(origin.port_),
      virtual_servers_(origin.virtual_servers_) {}

TcpServer::~TcpServer() {}

std::string TcpServer::getIp(void) const { return ip_; }
std::string TcpServer::getPort(void) const { return port_; }

/* append new virtual server in virtual_servers */
void TcpServer::appendServer(const ServerBlock &servers,
                             const HttpServer *virtual_server) {
  for (std::set<std::string>::iterator it = servers.server_names.begin();
       it != servers.server_names.end(); ++it) {
    virtual_servers_[*it] = virtual_server;
  }
}  // what if there is the same host ?

#include "TcpServer.hpp"

TcpServer::TcpServer(const std::string &key)
    : ip_(getIpFromKey(key)),
      port_(getPortFromKey(key)),
      default_server_(NULL) {}

TcpServer::TcpServer(const std::string &ip, const std::string &port)
    : ip_(ip), port_(port), default_server_(NULL) {}

TcpServer::TcpServer(const TcpServer &origin)
    : ip_(origin.ip_),
      port_(origin.port_),
      virtual_servers_(origin.virtual_servers_) {}

TcpServer::~TcpServer() {}

/*======================//
 Setter
========================*/

void TcpServer::setDefaultServer(HttpServer *default_server) {
  default_server_ = default_server;
}

/*======================//
 Getter
========================*/

std::string TcpServer::getIp(void) const { return ip_; }
std::string TcpServer::getPort(void) const { return port_; }
HttpServer *TcpServer::getDefaultServer(void) const { return default_server_; }
TcpServer::VirtualServerType TcpServer::getVirtualServers(void) const {
  return virtual_servers_;
}
/*======================//
 set TcpServer
========================*/

/* append new virtual server in virtual_servers
if there is a duplicated server configuration
then occur error, quit the server*/
void TcpServer::appendServer(const ServerBlock &servers,
                             HttpServer *virtual_server) {
  if (default_server_ == NULL) {
    setDefaultServer(virtual_server);
  }
  for (std::set<std::string>::iterator it = servers.server_names.begin();
       it != servers.server_names.end(); ++it) {
    if (virtual_servers_.find(*it) != virtual_servers_.end()) {
      Error::log("Server configuration duplicated", "", EXIT_FAILURE);
    }
    virtual_servers_[*it] = virtual_server;
  }
}

#include "ServerManager.hpp"

ServerManager::ServerManager(const Config &config) { registerServer(config); };

ServerManager::~ServerManager() {}

void ServerManager::registerServer(const Config &config) {
  std::vector<ServerBlock> servers = config.getServerBlocks();
  std::vector<Listen> listens;
  HttpServer *virtual_server;

  for (std::vector<ServerBlock>::iterator it = servers.begin();
       it != servers.end(); ++it) {
    listens = it->listens;
    virtual_server = new HttpServer(*it);

    for (std::vector<Listen>::iterator lit = listens.begin();
         lit != listens.end(); ++lit) {
      if (tcp_servers_.find(lit->server_socket_key) == tcp_servers_.end()) {
        tcp_servers_[lit->server_socket_key] =
            TcpServer(lit->host, lit->port, virtual_server);
        continue;
      }
      tcp_servers_[lit->server_socket_key].appendServer(virtual_server);
    }
  }
}
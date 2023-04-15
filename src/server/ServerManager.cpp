#include "ServerManager.hpp"

ServerManager::ServerManager(const Config &config) { registerServer(config); };

ServerManager::~ServerManager() {}

/*======================//
 initialize server
/*======================*/

/* register TcpServer,  */
void ServerManager::registerServer(const Config &config) {
  std::vector<ServerBlock> servers = config.getServerBlocks();
  std::vector<Listen> listens;
  TcpServer *tcp_server;

  for (std::vector<ServerBlock>::iterator it = servers.begin();
       it != servers.end(); ++it) {
    listens = it->listens;
    for (std::vector<Listen>::iterator lit = listens.begin();
         lit != listens.end(); ++lit) {
      tcp_server = seekTcpServer(lit->server_socket_key);
      tcp_server->appendServer(*it);
    }
  }
}

/* seek the TcpServer using key
unless there is TcpServer look forward, then create new on and return */
TcpServer *ServerManager::seekTcpServer(std::string key) {
  TcpServerType::iterator tcp_server = tcp_servers_.find(key);
  if (tcp_server == tcp_servers_.end()) {
    TcpServer *new_server =
        new TcpServer(getIpFromKey(key), getPortFromKey(key));
    tcp_servers_[key] = new_server;
    return new_server;
  }
  return tcp_server->second;
}
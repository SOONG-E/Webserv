#include "ServerManager.hpp"

ServerManager::ServerManager(const Config &config) { registerServer(config); };

ServerManager::~ServerManager() {}

/*======================//
 initialize server
/*======================*/

/* register TcpServer, HttpServer */
void ServerManager::registerServer(const Config &config) {
  std::vector<ServerBlock> servers = config.getServerBlocks();
  std::vector<Listen> listens;
  TcpServer *tcp_server;
  HttpServer *http_server;

  for (std::vector<ServerBlock>::iterator it = servers.begin();
       it != servers.end(); ++it) {
    listens = it->listens;
    http_server = createHttpServer(*it);

    for (std::vector<Listen>::iterator lit = listens.begin();
         lit != listens.end(); ++lit) {
      tcp_server = seekTcpServer(lit->server_socket_key);
      tcp_server->appendServer(*it, http_server);
    }
  }
}

/* seek the TcpServer using key and return
unless there is TcpServer look forward, then create new on and return */
TcpServer *ServerManager::seekTcpServer(const std::string &key) {
  TcpServerType::iterator tcp_server = tcp_servers_.find(key);

  if (tcp_server == tcp_servers_.end()) {
    return createTcpServer(key);
  }
  return tcp_server->second;
}

/* create new TcpServer */
TcpServer *ServerManager::createTcpServer(const std::string &key) {
  return new TcpServer(key);
}

/* create new HttpServer and append it to http_servers list */
HttpServer *ServerManager::createHttpServer(const ServerBlock &server_block) {
  HttpServer *http_server = new HttpServer(server_block);
  http_servers_.push_back(http_server);

  return http_server;
}
#include "ServerHandler.hpp"

ServerHandler::ServerHandler() {}

ServerHandler::ServerHandler(const ServerHandler &src) { *this = src; }

ServerHandler::~ServerHandler() {}

ServerHandler &ServerHandler::operator=(const ServerHandler &src) {
  if (this != &src) {
    _server_blocks = src._server_blocks;
    _server_sockets = src._server_sockets;
    _clients = src._clients;
    _server_selector = src._server_selector;
    _client_selector = src._client_selector;
  }
  return *this;
}

void ServerHandler::configureServer(const Config &config) {
  std::vector<ServerBlock> serv_info = config.getServerBlocks();

  for (size_t i = 0; i < serv_info.size(); ++i) {
    std::set<std::string> listens = serv_info[i].getListen().listen;
    for (std::set<std::string>::const_iterator it = listens.begin();
         it != listens.end(); ++it) {
      if (_server_blocks.find(*it) == _server_blocks.end()) {
        std::vector<ServerBlock> in(1, serv_info[i]);
        _server_blocks[*it] = in;
      } else {
        _server_blocks[*it].push_back(serv_info[i]);
      }
    }
  }
}

void ServerHandler::createServerSockets() {
  for (server_config_type::const_iterator it = _server_blocks.begin();
       it != _server_blocks.end(); ++it) {
    size_t pos = it->first.find(':');
    std::string ip = it->first.substr(0, pos);
    std::string port = it->first.substr(pos + 1);

    ServerSocket server_socket;
    server_socket.open();
    server_socket.bind(InetSocketAddress(ip, port), 128);
    _server_sockets.push_back(server_socket);

    _server_selector.registerSocket(server_socket.getSocket());
  }
}

void ServerHandler::acceptConnections() {
  if (_server_selector.select() > 0) {
    for (size_t i = 0; i < _server_sockets.size(); ++i) {
      if (_server_selector.isSetRead(_server_sockets[i].getSocket())) {
        Client new_client = _server_sockets[i].accept();
        _client_selector.registerSocket(new_client.getSocket());
        _clients.push_back(new_client);
      }
    }
  }
}

void ServerHandler::respondToClients() {
  if (_client_selector.select() > 0) {
    for (clients_type::iterator it = _clients.begin(); it != _clients.end();
         ++it) {
      if (_client_selector.isSetRead(
              (*it).getSocket()))  // 해당 클라이언트 소켓에 이벤트가 발생했으면
      {
        // 읽고 반환값0이면 연결 해제하고 읽은게 있으면 파싱하고 응답하는데
        // 짤려서 들어오면 한번 더 읽어야함.
      }
    }
  }
}

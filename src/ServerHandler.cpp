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
  const std::vector<ServerBlock> &serv_info = config.getServerBlocks();

  for (size_t i = 0; i < serv_info.size(); ++i) {
    const std::vector<Listen> &listens = serv_info[i].getListens();
    for (size_t i = 0; i < listens.size(); ++i) {
      if (_server_blocks.find(listens[i].socket_key) == _server_blocks.end()) {
        std::vector<ServerBlock> in(1, serv_info[i]);
        _server_blocks[listens[i].socket_key] = in;
      } else {
        _server_blocks[listens[i].socket_key].push_back(serv_info[i]);
      }
    }
  }
}

void ServerHandler::createServers() {
  for (server_blocks_type::const_iterator it = _server_blocks.begin();
       it != _server_blocks.end(); ++it) {
    size_t pos = it->first.find(':');
    std::string ip = it->first.substr(0, pos);
    std::string port = it->first.substr(pos + 1);

    ServerSocket server_socket;
    server_socket.open();
    server_socket.bind(SocketAddress(ip, port), 128);

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
        _clients.insert(std::make_pair(new_client.getSocket(), new_client));
      }
    }
  }
}

void ServerHandler::respondToClients() {
  if (_client_selector.select() > 0) {
    for (clients_type::iterator it = _clients.begin(); it != _clients.end();
         ++it) {
      Client *client = &it->second;

      if (_client_selector.isSetRead(client->getSocket())) {
        std::string buf = client->receive();
        if (buf.empty()) {
          closeConnection(client->getSocket());
          continue;
        }

        HttpParser &parser = client->getParser();
        const HttpRequest &request = parser.getRequest();

        if (parser.getBuffer().empty()) {
          parser.appendBuffer(buf);
          const ServerBlock *server_block =
              getServerBlock(client->getKey(), request.getHeader("Host"));
          client->setServerBlock(server_block);
        }
      }
    }
  }
}

const ServerBlock *ServerHandler::getServerBlock(
    const std::string &key, const std::string &server_name) {
  const std::vector<ServerBlock> &serv_blocks = _server_blocks[key];
  for (size_t i = 0; i < serv_blocks.size(); ++i) {
    const std::set<std::string> &serv_names = serv_blocks[i].getServerNames();
    if (serv_names.find(server_name) != serv_names.end()) {
      return &serv_blocks[i];
    }
  }
  return &serv_blocks[0];
}

void ServerHandler::closeConnection(int client_socket) {
  _clients.erase(client_socket);
  _client_selector.clear(client_socket);
  close(client_socket);
}
#include "ServerHandler.hpp"

#include "ResponseStatus.hpp"

ServerHandler::ServerHandler() {}

ServerHandler::ServerHandler(const ServerHandler &src) { *this = src; }

ServerHandler::~ServerHandler() {}

ServerHandler &ServerHandler::operator=(const ServerHandler &src) {
  if (this != &src) {
    server_blocks_ = src.server_blocks_;
    server_sockets_ = src.server_sockets_;
    clients_ = src.clients_;
    server_selector_ = src.server_selector_;
    client_selector_ = src.client_selector_;
  }
  return *this;
}

void ServerHandler::configureServer(const Config &config) {
  const std::vector<ServerBlock> &serv_info = config.getServerBlocks();

  for (size_t i = 0; i < serv_info.size(); ++i) {
    const std::vector<Listen> &listens = serv_info[i].getListens();

    for (size_t i = 0; i < listens.size(); ++i) {
      if (server_blocks_.find(listens[i].socket_key) == server_blocks_.end()) {
        std::vector<ServerBlock> in(1, serv_info[i]);

        server_blocks_[listens[i].socket_key] = in;
      } else {
        server_blocks_[listens[i].socket_key].push_back(serv_info[i]);
      }
    }
  }
}

void ServerHandler::createServers() {
  for (server_blocks_type::const_iterator it = server_blocks_.begin();
       it != server_blocks_.end(); ++it) {
    size_t pos = it->first.find(':');
    std::string ip = it->first.substr(0, pos);
    std::string port = it->first.substr(pos + 1);

    ServerSocket server_socket;
    server_socket.open();
    server_socket.bind(SocketAddress(ip, port), 128);

    server_sockets_.push_back(server_socket);

    server_selector_.registerSocket(server_socket.getSocket());
  }
}

void ServerHandler::acceptConnections() {
  if (server_selector_.select() > 0) {
    for (size_t i = 0; i < server_sockets_.size(); ++i) {
      if (server_selector_.isSetRead(server_sockets_[i].getSocket())) {
        Client new_client = server_sockets_[i].accept();

        client_selector_.registerSocket(new_client.getSocket());

        clients_.insert(std::make_pair(new_client.getSocket(), new_client));
      }
    }
  }
}

void ServerHandler::receiveRequest(Client *client, HttpParser &parser,
                                   int client_socket) {
  std::string request = client->receive();

  if (request.empty()) {
    closeConnection(client_socket);
    return;
  }

  try {
    parser.appendRequest(request);
  } catch (const HttpParser::BadRequestException &e) {
    client->getResponseObj().setStatus("400", ResponseStatus::REASONS[C400]);
  } catch (const HttpParser::LengthRequired &e) {
    client->getResponseObj().setStatus("411", ResponseStatus::REASONS[C411]);
  } catch (const HttpParser::PayloadTooLargeException &e) {
    client->getResponseObj().setStatus("413", ResponseStatus::REASONS[C413]);
  }
}

void ServerHandler::sendResponse(Client *client, HttpParser &parser) {
  const std::string &server_name = parser.getRequestObj().getHeader("Host");
  const std::string &socket_key = client->getSocketKey();
  const ServerBlock *server_block = getServerBlock(socket_key, server_name);

  client->send(server_block);

  if (!client->isPartialWritten()) {
    parser.clear();
  }
}

void ServerHandler::respondToClients() {
  if (client_selector_.select() > 0) {
    for (clients_type::iterator it = clients_.begin(); it != clients_.end();
         ++it) {
      Client *client = &it->second;
      HttpParser parser = client->getParser();
      int client_socket = client->getSocket();

      if (client_selector_.isSetRead(client_socket)) {
        receiveRequest(client, parser, client_socket);
      }
      if (client_selector_.isSetWrite(client_socket) &&
          (parser.isCompleted() ||
           client->getResponseObj().getCode() != "200")) {
        sendResponse(client, parser);
      }
    }
  }
}

const ServerBlock *ServerHandler::getServerBlock(
    const std::string &key, const std::string &server_name) {
  const std::vector<ServerBlock> &blocks_of_key = server_blocks_[key];
  for (size_t i = 0; i < blocks_of_key.size(); ++i) {
    const std::set<std::string> &names_of_key =
        blocks_of_key[i].getServerNames();
    if (names_of_key.find(server_name) != names_of_key.end()) {
      return &blocks_of_key[i];
    }
  }
  return &blocks_of_key[0];
}

void ServerHandler::closeConnection(int client_socket) {
  clients_.erase(client_socket);
  client_selector_.clear(client_socket);
  close(client_socket);
}

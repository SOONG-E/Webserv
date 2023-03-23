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
  const std::vector<ServerBlock> &serv_blocks = config.getServerBlocks();

  for (size_t i = 0; i < serv_blocks.size(); ++i) {
    const std::vector<Listen> &listens = serv_blocks[i].getListens();
    for (size_t i = 0; i < listens.size(); ++i) {
      if (server_blocks_.find(listens[i].socket_key) == server_blocks_.end()) {
        std::vector<ServerBlock> in(1, serv_blocks[i]);
        server_blocks_[listens[i].socket_key] = in;
      } else {
        server_blocks_[listens[i].socket_key].push_back(serv_blocks[i]);
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
    server_selector_.registerFD(server_socket.getFD());
  }
}

void ServerHandler::acceptConnections() {
  if (server_selector_.select() > 0) {
    for (size_t i = 0; i < server_sockets_.size(); ++i) {
      if (server_selector_.isSetRead(server_sockets_[i].getFD())) {
        try {
          Client new_client = server_sockets_[i].accept();

          client_selector_.registerFD(new_client.getFD());
          clients_.insert(std::make_pair(new_client.getFD(), new_client));
        } catch (const std::exception &e) {
          std::cerr << "[Error] Connection failed: " << e.what() << '\n';
        }
      }
    }
  }
}

void ServerHandler::respondToClients() {
  if (client_selector_.select() > 0) {
    std::vector<int> delete_clients;

    delete_clients.reserve(clients_.size());

    for (clients_type::iterator it = clients_.begin(); it != clients_.end();
         ++it) {
      Client *client = &it->second;

      if (client_selector_.isSetRead(client->getFD())) {
        receiveRequest(client, delete_clients);
      }
      if (client_selector_.isSetWrite(client->getFD()) &&
          (client->isParseCompleted() || !client->isResponseSuccess())) {
        sendResponse(client, delete_clients);
      }
    }

    if (!delete_clients.empty()) {
      deleteClients(delete_clients);
    }
  }
}

bool ServerHandler::isImplementedMethod(std::string method) {
  for (std::size_t i = 0; i < METHODS_COUNT; ++i) {
    if (METHODS[i] == method) {
      return true;
    }
  }
  return false;
}

void ServerHandler::receiveRequest(Client *client,
                                   std::vector<int> &delete_clients) {
  try {
    std::string request = client->receive();
    if (request.empty()) {
      delete_clients.push_back(client->getFD());
      closeConnection(client);
      return;
    }
    client->appendRequest(request);
  } catch (const HttpParser::BadRequestException &e) {
    client->setResponseStatus("400", ResponseStatus::REASONS[C400]);
  } catch (const HttpParser::LengthRequired &e) {
    client->setResponseStatus("411", ResponseStatus::REASONS[C411]);
  } catch (const HttpParser::PayloadTooLargeException &e) {
    client->setResponseStatus("413", ResponseStatus::REASONS[C413]);
  } catch (const HttpParser::HttpVersionNotSupportedException &e) {
    client->setResponseStatus("505", ResponseStatus::REASONS[C505]);
  } catch (const std::exception &e) {
    client->setResponseStatus("500", ResponseStatus::REASONS[C500]);
  }
  if (!isImplementedMethod(client->getRequestMethod())) {
    client->setResponseStatus("501", ResponseStatus::REASONS[C501]);
  }
}

void ServerHandler::sendResponse(Client *client,
                                 std::vector<int> &delete_clients) {
  std::string server_name = client->getRequestHeader("Host");
  const std::string &socket_key = client->getSocketKey();
  const ServerBlock *server_block = getServerBlock(socket_key, server_name);

  try {
    client->send(server_block);
    if (!client->isPartialWritten()) {
      std::string connection = client->getRequestHeader("Connection");

      if (connection == "close" || !client->isResponseSuccess()) {
        delete_clients.push_back(client->getFD());
        closeConnection(client);
        return;
      }
      client->clearParser();
    }
  } catch (const std::exception &e) {
    client->setResponseStatus("500", ResponseStatus::REASONS[C500]);
    client->clearResponseBuf();
    std::cerr << "[Error] Send failed: " << e.what() << '\n';
  }
}

const ServerBlock *ServerHandler::getServerBlock(
    const std::string &socket_key, const std::string &server_name) {
  const std::vector<ServerBlock> &server_blocks = server_blocks_[socket_key];

  for (size_t i = 0; i < server_blocks.size(); ++i) {
    const std::set<std::string> &server_names =
        server_blocks[i].getServerNames();

    if (server_names.find(server_name) != server_names.end()) {
      return &server_blocks[i];
    }
  }
  return &server_blocks[0];
}

void ServerHandler::closeConnection(Client *client) {
  close(client->getFD());

  Log::header("Close Connection Information");
  client->logAddressInfo();
  Log::footer();
}

void ServerHandler::deleteClients(const std::vector<int> &delete_clients) {
  for (size_t i = 0; i < delete_clients.size(); ++i) {
    clients_.erase(delete_clients[i]);
    client_selector_.clear(delete_clients[i]);
  }
}

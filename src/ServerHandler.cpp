#include "ServerHandler.hpp"

#include "ResponseException.hpp"
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
      if (server_selector_.isReadable(server_sockets_[i].getFD())) {
        try {
          Client new_client = server_sockets_[i].accept();
          const ServerBlock &server_block =
              findServerBlock(new_client.getSocketKey(), "");

          new_client.getResponseObj().setServerBlock(&server_block);

          int client_fd = new_client.getFD();

          client_selector_.registerFD(client_fd);
          clients_.insert(std::make_pair(client_fd, new_client));
        } catch (const std::exception &e) {
          std::cerr << "[Error] Connection failed: " << e.what() << '\n';
        }
      }
    }
  }
}

void ServerHandler::respondToClients() {
  if (client_selector_.select() > 0) {
    Client *client;
    std::vector<int> delete_clients;

    delete_clients.reserve(clients_.size());

    for (clients_type::iterator it = clients_.begin(); it != clients_.end();
         ++it) {
      client = &it->second;
      int client_fd = client->getFD();

      if (client_selector_.isReadable(client_fd)) {
        receiveRequest(*client, delete_clients);
      }
      if (client_selector_.isWritable(client_fd) &&
          (client->getParser().isCompleted() ||
           !client->getResponseObj().isSuccessCode())) {
        sendResponse(*client, delete_clients);
      }
    }

    if (!delete_clients.empty()) {
      deleteClients(delete_clients);
    }
  }
}

void ServerHandler::receiveRequest(Client &client,
                                   std::vector<int> &delete_clients) {
  try {
    std::string request = client.receive();

    if (request.empty()) {
      delete_clients.push_back(client.getFD());
      closeConnection(client);
      return;
    }

    HttpParser &parser = client.getParser();
    parser.appendRequest(request);

    if (parser.isCompleted()) {
      const HttpRequest &request_obj = client.getRequestObj();
      const ServerBlock &server_block =
          findServerBlock(client.getSocketKey(), request_obj.getHeader("Host"));
      const LocationBlock &location_block =
          server_block.findLocationBlock(request_obj.getUri());

      HttpResponse &response_obj = client.getResponseObj();
      response_obj.setServerBlock(&server_block);
      response_obj.setLocationBlock(&location_block);

      validateRequest(request_obj, location_block);
    }
  } catch (const ResponseException &e) {
    client.getResponseObj().setStatus(e.index);
  }
}

void ServerHandler::sendResponse(Client &client,
                                 std::vector<int> &delete_clients) {
  try {
    client.send();
    if (!client.isPartialWritten()) {
      const HttpRequest &request_obj = client.getRequestObj();
      HttpResponse &response_obj = client.getResponseObj();
      HttpParser &parser = client.getParser();

      if (request_obj.getHeader("Connection") == "close" ||
          !response_obj.isSuccessCode()) {
        delete_clients.push_back(client.getFD());
        closeConnection(client);
        return;
      }
      parser.clear();
      response_obj.clear();
    }
  } catch (const std::exception &e) {
    client.getResponseObj().setStatus(C500);
    client.clearBuffer();
    std::cerr << "[Error] Send failed: " << e.what() << '\n';
  }
}

void ServerHandler::validateRequest(const HttpRequest &request_obj,
                                    const LocationBlock &location_block) {
  const std::string &request_method = request_obj.getMethod();
  if (!location_block.isImplementedMethod(request_method)) {
    throw ResponseException(C501);
  } else if (!location_block.isAllowedMethod(request_method)) {
    throw ResponseException(C405);
  } else if (request_method == "POST" &&
             location_block.body_limit < request_obj.getContentLength()) {
    throw ResponseException(C413);
  }
}

const ServerBlock &ServerHandler::findServerBlock(
    const std::string &socket_key, const std::string &server_name) {
  const std::vector<ServerBlock> &server_blocks = server_blocks_[socket_key];

  for (size_t i = 0; i < server_blocks.size(); ++i) {
    const std::set<std::string> &server_names =
        server_blocks[i].getServerNames();

    if (server_names.find(server_name) != server_names.end()) {
      return server_blocks[i];
    }
  }
  return server_blocks[0];
}

void ServerHandler::closeConnection(Client &client) {
  close(client.getFD());

  Log::header("Close Connection Information");
  client.logAddressInfo();
  Log::footer("Close Connection");
}

void ServerHandler::deleteClients(const std::vector<int> &delete_clients) {
  for (size_t i = 0; i < delete_clients.size(); ++i) {
    clients_.erase(delete_clients[i]);
    client_selector_.clear(delete_clients[i]);
  }
}

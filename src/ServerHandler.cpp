#include "ServerHandler.hpp"

#include <unistd.h>

#include <cstdlib>
#include <iostream>

#include "Log.hpp"
#include "ResponseStatus.hpp"
#include "exception.hpp"

ServerHandler::ServerHandler() {}

ServerHandler::ServerHandler(const ServerHandler& src)
    : server_blocks_(src.server_blocks_),
      server_sockets_(src.server_sockets_),
      clients_(src.clients_),
      server_selector_(src.server_selector_),
      client_selector_(src.client_selector_) {}

ServerHandler::~ServerHandler() {}

void ServerHandler::registerSignalHandlers() const {
  signal(SIGPIPE, SIG_IGN);
  signal(SIGCHLD, SIG_IGN);
}

void ServerHandler::configureServer(const Config& config) {
  const std::vector<ServerBlock>& serv_blocks = config.getServerBlocks();

  for (std::size_t i = 0; i < serv_blocks.size(); ++i) {
    const std::vector<Listen>& listens = serv_blocks[i].getListens();
    for (std::size_t i = 0; i < listens.size(); ++i) {
      try {
        server_blocks_[listens[i].socket_key].push_back(serv_blocks[i]);
      } catch (const std::exception& e) {
        std::cerr << "[Error] configureServer failed: " << e.what() << '\n';
        std::exit(EXIT_FAILURE);
      }
    }
  }
}

void ServerHandler::createServers() {
  for (server_blocks_type::const_iterator it = server_blocks_.begin();
       it != server_blocks_.end(); ++it) {
    const std::string& socket_key = it->first;
    const ServerBlock& default_server = server_blocks_[socket_key].front();

    ServerSocket server_socket(default_server);
    try {
      server_socket.open();

      std::size_t pos = socket_key.find(':');
      std::string ip = socket_key.substr(0, pos);
      std::string port = socket_key.substr(pos + 1);
      server_socket.bind(SocketAddress(ip, port), 128);

      server_sockets_.push_back(server_socket);
      server_selector_.registerFD(server_socket.getFD());
    } catch (const std::exception& e) {
      std::cerr << "[Error] createServers failed: " << e.what() << '\n';
      std::exit(EXIT_FAILURE);
    }
  }
}

void ServerHandler::acceptConnections() {
  try {
    if (server_selector_.select() > 0) {
      for (std::size_t i = 0; i < server_sockets_.size(); ++i) {
        if (server_selector_.isReadable(server_sockets_[i].getFD())) {
          Client new_client = server_sockets_[i].accept();

          client_selector_.registerFD(new_client.getFD());
          clients_.insert(std::make_pair(new_client.getFD(), new_client));
        }
      }
    }
  } catch (const std::exception& e) {
    std::cerr << "[Error] acceptConnections failed: " << e.what() << '\n';
  }
}

void ServerHandler::respondToClients() {
  try {
    if (client_selector_.select() > 0) {
      Client* client;
      std::vector<int> delete_clients;

      delete_clients.reserve(clients_.size());

      for (clients_type::iterator it = clients_.begin(); it != clients_.end();
           ++it) {
        client = &it->second;
        int client_fd = client->getFD();

        try {
          if (client_selector_.isReadable(client_fd)) {
            receiveRequest(*client);
          }
          if (client->isReadyToCgiIO()) {
            client->executeCgiIO();
          }
          if (client_selector_.isWritable(client_fd) &&
              client->isReadyToSend()) {
            sendResponse(*client);
          }
        } catch (const Client::ConnectionClosedException& e) {
          delete_clients.push_back(client->getFD());
          client->closeConnection();
        }
      }

      if (!delete_clients.empty()) {
        deleteClients(delete_clients);
      }
    }
  } catch (const std::exception& e) {
    std::cerr << "[Error] respondToClients failed: " << e.what() << '\n';
  }
}

void ServerHandler::receiveRequest(Client& client) {
  try {
    HttpParser& parser = client.getParser();
    parser.appendRequest(client.receive());

    if (parser.isCompleted()) {
      const HttpRequest& request_obj = client.getRequestObj();
      const ServerBlock& server_block =
          findServerBlock(client.getSocketKey(), request_obj.getHeader("HOST"));
      const LocationBlock& location_block =
          server_block.findLocationBlock(request_obj.getUri());

      HttpResponse& response_obj = client.getResponseObj();
      response_obj.setServerBlock(&server_block);
      response_obj.setLocationBlock(&location_block);

      validateRequest(request_obj, location_block);

      if (client.isCgi()) {
        client.getCgi().runCgiScript(request_obj, client.getClientAddress(),
                                     client.getServerAddress(),
                                     location_block.getCgiParam("CGI_PATH"));
      }
    }
  } catch (const ResponseException& e) {
    client.getResponseObj().setStatus(e.index);
  } catch (const std::exception& e) {
    client.getResponseObj().setStatus(C500);
    std::cerr << "[Error] receiveRequest failed: " << e.what() << '\n';
  }
}

void ServerHandler::sendResponse(Client& client) {
  try {
    client.send();
    if (!client.isPartialWritten()) {
      const HttpRequest& request_obj = client.getRequestObj();
      const HttpResponse& response_obj = client.getResponseObj();

      if (request_obj.getHeader("CONNECTION") == "close" ||
          !response_obj.isSuccessCode()) {
        throw Client::ConnectionClosedException();
      }
      client.clear();
    }
  } catch (const Client::ConnectionClosedException& e) {
    throw Client::ConnectionClosedException();
  } catch (const std::exception& e) {
    client.getResponseObj().setStatus(C500);
    std::cerr << "[Error] sendResponse failed: " << e.what() << '\n';
  }
}

const ServerBlock& ServerHandler::findServerBlock(
    const std::string& socket_key, const std::string& server_name) {
  const std::vector<ServerBlock>& server_blocks_of_key =
      server_blocks_[socket_key];

  for (std::size_t i = 0; i < server_blocks_of_key.size(); ++i) {
    const std::set<std::string>& server_names =
        server_blocks_of_key[i].getServerNames();

    if (server_names.find(server_name) != server_names.end()) {
      return server_blocks_of_key[i];
    }
  }
  return server_blocks_of_key[0];
}

void ServerHandler::validateRequest(const HttpRequest& request_obj,
                                    const LocationBlock& location_block) {
  const std::string& request_method = request_obj.getMethod();
  if (!location_block.isImplementedMethod(request_method)) {
    throw ResponseException(C501);
  } else if (!location_block.isAllowedMethod(request_method)) {
    throw ResponseException(C405);
  } else if (request_method == METHODS[POST] &&
             location_block.getBodyLimit() < request_obj.getContentLength()) {
    throw ResponseException(C413);
  }
}

void ServerHandler::deleteClients(const std::vector<int>& delete_clients) {
  for (std::size_t i = 0; i < delete_clients.size(); ++i) {
    clients_.erase(delete_clients[i]);
    client_selector_.clear(delete_clients[i]);
  }
}
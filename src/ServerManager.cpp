#include "ServerManager.hpp"

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>

#include "Error.hpp"
#include "Log.hpp"
#include "ResponseStatus.hpp"
#include "constant.hpp"
#include "exception.hpp"

ServerManager::ServerManager() {}

ServerManager::ServerManager(const ServerManager& src)
    : server_blocks_(src.server_blocks_),
      server_sockets_(src.server_sockets_),
      clients_(src.clients_),
      selector_(src.selector_),
      session_handler_(src.session_handler_) {}

ServerManager::~ServerManager() {}

void ServerManager::registerSignalHandlers() const {
  signal(SIGPIPE, SIG_IGN);
  signal(SIGCHLD, SIG_IGN);
}

void ServerManager::configureServer(const Config& config) {
  const std::vector<ServerBlock>& serv_blocks = config.getServerBlocks();

  for (std::size_t i = 0; i < serv_blocks.size(); ++i) {
    const std::vector<Listen>& listens = serv_blocks[i].getListens();
    for (std::size_t j = 0; j < listens.size(); ++j) {
      try {
        server_blocks_[listens[j].server_socket_key].push_back(serv_blocks[i]);
      } catch (const std::exception& e) {
        Error::log("configureServer() failed", e.what(), EXIT_FAILURE);
      }
    }
  }
}

void ServerManager::createServers() {
  for (server_blocks_type::const_iterator it = server_blocks_.begin();
       it != server_blocks_.end(); ++it) {
    const std::string& server_socket_key = it->first;
    const ServerBlock& default_server =
        server_blocks_[server_socket_key].front();

    ServerSocket server_socket(default_server);
    try {
      server_socket.open();

      std::size_t pos = server_socket_key.find(':');
      std::string ip = server_socket_key.substr(0, pos);
      std::string port = server_socket_key.substr(pos + 1);
      server_socket.bind(SocketAddress(ip, port), 128);

      server_sockets_.push_back(server_socket);
      selector_.registerFD(server_socket.getFD());
    } catch (const std::exception& e) {
      Error::log("createServers() failed", e.what(), EXIT_FAILURE);
    }
  }
}

int ServerManager::select() { return selector_.select(); }

void ServerManager::acceptConnections() {
  try {
    for (std::size_t i = 0; i < server_sockets_.size(); ++i) {
      if (selector_.isReadable(server_sockets_[i].getFD())) {
        Client new_client = server_sockets_[i].accept();

        int client_fd = new_client.getFD();
        selector_.registerFD(client_fd);
        clients_.insert(std::make_pair(client_fd, new_client));
      }
    }
  } catch (const std::exception& e) {
    // Error::log("acceptConnections() failed", e.what());
  }
}

void ServerManager::respondToClients() {
  try {
    Client* client;
    std::vector<int> delete_clients;

    delete_clients.reserve(clients_.size());

    for (clients_type::iterator it = clients_.begin(); it != clients_.end();
         ++it) {
      client = &it->second;
      int client_fd = client->getFD();
      try {
        if (client->isReadyToCgiIO()) {
          client->executeCgiIO(selector_);
        }
        if (selector_.isReadable(client_fd)) {
          receiveRequest(*client);
        }
        if (selector_.isWritable(client_fd) && client->isReadyToSend()) {
          sendResponse(*client);
        }
      } catch (const Client::ConnectionClosedException& e) {
        delete_clients.push_back(client_fd);
        client->closeConnection();
      }
    }
    if (!delete_clients.empty()) {
      deleteClients(delete_clients);
    }
  } catch (const std::exception& e) {
    // Error::log("respondToClients() failed", e.what());
  }
}

void ServerManager::handleTimeout() {
  deleteTimeoutClients();
  session_handler_.deleteTimeoutSessions();
}

void ServerManager::receiveRequest(Client& client) {
  HttpResponse& response_obj = client.getResponseObj();
  try {
    std::string request = client.receive();
    client.setTimeout();

    HttpParser& parser = client.getParser();
    parser.appendRequest(request);

    if (parser.isCompleted()) {
      const HttpRequest& request_obj = client.getRequestObj();
      const ServerBlock& server_block = findServerBlock(
          client.getServerSocketKey(), request_obj.getHeader("HOST"));
      const LocationBlock& location_block =
          server_block.findLocationBlock(request_obj.getUri());

      response_obj.setServerBlock(&server_block);
      response_obj.setLocationBlock(&location_block);

      validateRequest(request_obj, location_block);

      Session* session = session_handler_.findSession(client);
      if (!session) {
        session = session_handler_.generateSession(client);
      }
      response_obj.setSession(session);

      if (client.isCgi()) {
        client.runCgiProcess(selector_);
      }
    }
  } catch (const ResponseException& e) {
    response_obj.setStatus(e.status);
  } catch (const Client::ConnectionClosedException& e) {
    throw Client::ConnectionClosedException();
  } catch (const std::exception& e) {
    response_obj.setStatus(C500);
  }
}

void ServerManager::sendResponse(Client& client) {
  HttpResponse& response_obj = client.getResponseObj();
  try {
    client.setTimeout();
    client.setSessionTimeout();

    client.send();
    if (!client.isPartialWritten()) {
      if (client.getRequestObj().getHeader("CONNECTION") == "close" ||
          !response_obj.isSuccessCode()) {
        throw Client::ConnectionClosedException();
      }
      client.clear();
    }
  } catch (const Client::ConnectionClosedException& e) {
    throw Client::ConnectionClosedException();
  } catch (const std::exception& e) {
    response_obj.setStatus(C500);
  }
}

const ServerBlock& ServerManager::findServerBlock(
    const std::string& server_socket_key, const std::string& server_name) {
  const std::vector<ServerBlock>& server_blocks_of_key =
      server_blocks_[server_socket_key];

  for (std::size_t i = 0; i < server_blocks_of_key.size(); ++i) {
    const std::set<std::string>& server_names =
        server_blocks_of_key[i].getServerNames();

    if (server_names.find(server_name) != server_names.end()) {
      return server_blocks_of_key[i];
    }
  }
  return server_blocks_of_key[0];
}

void ServerManager::validateRequest(const HttpRequest& request_obj,
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

void ServerManager::deleteTimeoutClients() {
  Client* client;
  std::vector<int> delete_clients;

  delete_clients.reserve(clients_.size());

  for (clients_type::iterator it = clients_.begin(); it != clients_.end();
       ++it) {
    client = &it->second;
    if (client->getTimeout() < std::time(0)) {
      client->closeConnection();
      delete_clients.push_back(client->getFD());
    }
  }
  if (!delete_clients.empty()) {
    deleteClients(delete_clients);
  }
}

void ServerManager::deleteClients(const std::vector<int>& delete_clients) {
  for (std::size_t i = 0; i < delete_clients.size(); ++i) {
    clients_.erase(delete_clients[i]);
    selector_.unregisterFD(delete_clients[i]);
  }
}
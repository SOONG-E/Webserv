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
    std::queue<int> delete_clients;

    for (clients_type::iterator it = clients_.begin(); it != clients_.end();
         ++it) {
      client = &it->second;
      int client_fd = client->getFD();
      try {
        if (client->isReadyToCgiIO()) {
          client->executeCgiIO(selector_);
        }
        if (selector_.isReadable(client_fd)) {
          this->receiveRequest(*client);
        }
        if (selector_.isWritable(client_fd) && client->isReadyToSend()) {
          this->sendResponse(*client);
        }
      } catch (const Client::ConnectionClosedException& e) {
        delete_clients.push(client_fd);
        client->closeConnection();
      }
    }
    this->deleteClients(delete_clients);
  } catch (const std::exception& e) {
    // Error::log("respondToClients() failed", e.what());
  }
}

void ServerManager::handleTimeout() {
  this->deleteTimeoutClients();
  session_handler_.deleteTimeoutSessions();
}

void ServerManager::setBlock(Client& client) {
  HttpRequest& request = client.getRequest();
  HttpResponse& response = request.getResponse();

  const ServerBlock server_block = this->findServerBlock(
      client.getServerSocketKey(), request.getHeader("HOST"));
  const LocationBlock location_block =
      server_block.findLocationBlock(request.getUri());

  response.setServerBlock(server_block);
  response.setLocationBlock(location_block);
  response.setFullUri(completeUri(request.getUri(), location_block));
}

void ServerManager::deleteFile(HttpResponse& response) {
  const std::string uri = response.getFullUri();
  if (unlink(uri.c_str()) == ERROR<int>()) {
    response.setStatus(C404);
    return;
  }
  response.setStatus(C204);
}

void ServerManager::preprocess(Client& client) {
  if (client.getRequest().getMethod() == METHODS[DELETE]) {
    deleteFile(client.getRequest().getResponse());
  }
  if (client.isCgi()) {
    client.runCgiProcess(selector_);
  }
}

void ServerManager::receiveRequest(Client& client) {
  HttpRequest& request = client.getRequest();
  try {
    std::string receivedMessage = client.receive();
    client.setTimeout();

    request.tailRequest(receivedMessage);
    request.parse();

    if (request.isCompleted() == true) {
      setBlock(client);
      validateRequest(request);

      Session* session = session_handler_.findSession(client);
      if (!session) {
        session = session_handler_.generateSession(client);
      }
      // response_obj.setSession(session);

      preprocess(client);
    }
  } catch (const ResponseException& e) {
    request.setStatus(e.status);
  } catch (const Client::ConnectionClosedException& e) {
    throw Client::ConnectionClosedException();
  } catch (const std::exception& e) {
    request.setStatus(C500);
  }
}

void ServerManager::sendResponse(Client& client) {
  HttpResponse& response_obj = client.getRequest().getResponse();
  try {
    client.setTimeout();
    client.setSessionTimeout();

    client.send();
    if (!client.isPartialResponse()) {
      if (client.getRequest().getHeader("CONNECTION") == "close" ||
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

void ServerManager::validateRequest(const HttpRequest& request) {
  const LocationBlock& location_block =
      request.getResponse().getLocationBlock();
  const std::string& request_method = request.getMethod();
  if (!location_block.isImplementedMethod(request_method)) {
    throw ResponseException(C501);
  } else if (!location_block.isAllowedMethod(request_method)) {
    throw ResponseException(C405);
  } else if (request_method == METHODS[POST] &&
             location_block.getBodyLimit() < request.getContentLength()) {
    throw ResponseException(C413);
  }
}

void ServerManager::deleteTimeoutClients() {
  Client* client;
  std::queue<int> delete_clients;

  for (clients_type::iterator it = clients_.begin(); it != clients_.end();
       ++it) {
    client = &it->second;
    if (client->getTimeout() < std::time(0)) {
      client->closeConnection();
      delete_clients.push(client->getFD());
    }
  }
  this->deleteClients(delete_clients);
}

void ServerManager::deleteClients(std::queue<int>& delete_clients) {
  while (!delete_clients.empty()) {
    clients_.erase(delete_clients.front());
    selector_.unregisterFD(delete_clients.front());
    delete_clients.pop();
  }
}

std::string ServerManager::completeUri(std::string& uri,
                                       const LocationBlock& location_block) {
  std::string root = location_block.getRoot();
  if (*root.rbegin() != '/') {
    root += "/";
  }
  uri.replace(0, location_block.getUri().size(), root);
  return uri;
}
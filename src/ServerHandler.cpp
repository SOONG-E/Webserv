#include "ServerHandler.hpp"

#include <unistd.h>

#include <cstdlib>
#include <iostream>

#include "Error.hpp"
#include "Log.hpp"
#include "ResponseStatus.hpp"
#include "constant.hpp"
#include "exception.hpp"

std::size_t ServerHandler::avail_session_id_ = 0;

ServerHandler::ServerHandler() {}

ServerHandler::ServerHandler(const ServerHandler& src)
    : server_blocks_(src.server_blocks_),
      server_sockets_(src.server_sockets_),
      clients_(src.clients_),
      server_selector_(src.server_selector_),
      client_selector_(src.client_selector_),
      sessions_(src.sessions_) {}

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
        server_blocks_[listens[i].server_key].push_back(serv_blocks[i]);
      } catch (const std::exception& e) {
        Error::log("configureServer() failed", e.what(), EXIT_FAILURE);
      }
    }
  }
}

void ServerHandler::createServers() {
  for (server_blocks_type::const_iterator it = server_blocks_.begin();
       it != server_blocks_.end(); ++it) {
    const std::string& server_key = it->first;
    const ServerBlock& default_server = server_blocks_[server_key].front();

    ServerSocket server_socket(default_server);
    try {
      server_socket.open();

      std::size_t pos = server_key.find(':');
      std::string ip = server_key.substr(0, pos);
      std::string port = server_key.substr(pos + 1);
      server_socket.bind(SocketAddress(ip, port), 128);

      server_sockets_.push_back(server_socket);
      server_selector_.registerFD(server_socket.getFD());
    } catch (const std::exception& e) {
      Error::log("createServers() failed", e.what(), EXIT_FAILURE);
    }
  }
}

void ServerHandler::acceptConnections() {
  try {
    if (server_selector_.select() > 0) {
      for (std::size_t i = 0; i < server_sockets_.size(); ++i) {
        if (server_selector_.isReadable(server_sockets_[i].getFD())) {
          Client new_client = server_sockets_[i].accept();
          int client_fd = new_client.getFD();

          client_selector_.registerFD(client_fd);
          clients_.insert(std::make_pair(client_fd, new_client));
        }
      }
    }
  } catch (const std::exception& e) {
    Error::log("acceptConnections() failed", e.what());
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
          delete_clients.push_back(client_fd);
          client->closeConnection();
        }
      }

      if (!delete_clients.empty()) {
        deleteClients(delete_clients);
      }
    }
  } catch (const std::exception& e) {
    Error::log("respondToClients() failed", e.what());
  }
}

void ServerHandler::handleTimeout() {
  Client* client;
  std::vector<int> delete_clients;

  delete_clients.reserve(clients_.size());

  for (clients_type::iterator it = clients_.begin(); it != clients_.end();
       ++it) {
    client = &it->second;

    if (client->getTimeout() < std::time(0) && !client->isReceiveFinished()) {
      client->closeConnection();
      delete_clients.push_back(client->getFD());
    }
  }
  if (!delete_clients.empty()) {
    deleteClients(delete_clients);
  }

  Session* session;
  std::vector<std::string> delete_sessions;

  delete_sessions.reserve(sessions_.size());

  for (sessions_type::iterator it = sessions_.begin(); it != sessions_.end();
       ++it) {
    session = &it->second;

    if (session->getTimeout() < std::time(NULL)) {
      delete_sessions.push_back(it->first);
    }
  }
  if (!delete_sessions.empty()) {
    deleteSessions(delete_sessions);
  }
}

void ServerHandler::receiveRequest(Client& client) {
  try {
    std::string request = client.receive();
    client.setTimeout();

    HttpParser& parser = client.getParser();
    parser.appendRequest(request);

    if (parser.isCompleted()) {
      const HttpRequest& request_obj = client.getRequestObj();
      const ServerBlock& server_block =
          findServerBlock(client.getServerKey(), request_obj.getHeader("HOST"));
      const LocationBlock& location_block =
          server_block.findLocationBlock(request_obj.getUri());

      HttpResponse& response_obj = client.getResponseObj();
      response_obj.setServerBlock(&server_block);
      response_obj.setLocationBlock(&location_block);

      validateRequest(request_obj, location_block);

      if (!client.hasCookie() || !isValidSessionID(client)) {
        generateSession(client);
      }
      Session& session = findSession(client);
      client.setSession(session);

      if (request_obj.getMethod() == METHODS[DELETE]) {
        if (unlink(("." + request_obj.getUri()).c_str()) == ERROR<int>()) {
          response_obj.setStatus(C404);
        } else {
          response_obj.setStatus(C204);
        }
        return;
      }

      if (client.isCgi()) {
        client.getCgi().runCgiScript(request_obj, client.getClientAddress(),
                                     client.getServerAddress(),
                                     location_block.getCgiParam("CGI_PATH"));
      }
    }
  } catch (const ResponseException& e) {
    client.getResponseObj().setStatus(e.status);
  }
}

void ServerHandler::sendResponse(Client& client) {
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
  client.setTimeout();
  client.getSession().setTimeout();
}

const ServerBlock& ServerHandler::findServerBlock(
    const std::string& server_key, const std::string& server_name) {
  const std::vector<ServerBlock>& server_blocks_of_key =
      server_blocks_[server_key];

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
    client_selector_.unregisterFD(delete_clients[i]);
  }
}

void ServerHandler::deleteSessions(
    const std::vector<std::string>& delete_sessions) {
  for (std::size_t i = 0; i < delete_sessions.size(); ++i) {
    sessions_.erase(delete_sessions[i]);
  }
}

void ServerHandler::generateSession(Client& client) {
  std::string session_id = toString(avail_session_id_++);
  std::string session_key = session_id + ":" +
                            client.getClientAddress().getIP() + ":" +
                            client.getRequestObj().getHeader("USER-AGENT");

  sessions_.insert(std::make_pair(session_key, Session(session_id)));
}

bool ServerHandler::isValidSessionID(const Client& client) {
  const Session* session;
  const HttpRequest& request_obj = client.getRequestObj();
  std::string session_id = request_obj.getCookie("Session-ID");
  std::string session_key = session_id + ":" +
                            client.getClientAddress().getIP() + ":" +
                            request_obj.getHeader("USER-AGENT");

  for (sessions_type::iterator it = sessions_.begin(); it != sessions_.end();
       ++it) {
    session = &it->second;
    if (session->getSessionID() == session_id && it->first == session_key) {
      return true;
    }
  }
  return false;
}

Session& ServerHandler::findSession(const Client& client) {
  const HttpRequest& request_obj = client.getRequestObj();
  std::string session_key = request_obj.getCookie("Session-ID") + ":" +
                            client.getClientAddress().getIP() + ":" +
                            request_obj.getHeader("USER-AGENT");
  return sessions_.find(session_key)->second;
}

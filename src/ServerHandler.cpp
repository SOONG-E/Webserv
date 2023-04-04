#include "ServerHandler.hpp"

#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>

#include "Error.hpp"
#include "Log.hpp"
#include "ResponseStatus.hpp"
#include "constant.hpp"
#include "exception.hpp"

ServerHandler::ServerHandler() { std::srand(std::time(NULL)); }

ServerHandler::ServerHandler(const ServerHandler& src)
    : server_blocks_(src.server_blocks_),
      server_sockets_(src.server_sockets_),
      clients_(src.clients_),
      reserve_clients_(src.reserve_clients_),
      selector_(src.selector_),
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
    for (std::size_t j = 0; j < listens.size(); ++j) {
      try {
        server_blocks_[listens[j].server_socket_key].push_back(serv_blocks[i]);
      } catch (const std::exception& e) {
        Error::log("configureServer() failed", e.what(), EXIT_FAILURE);
      }
    }
  }
}

void ServerHandler::createServers() {
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

int ServerHandler::select() { return selector_.select(); }

void ServerHandler::acceptConnections() {
  try {
    for (std::size_t i = 0; i < server_sockets_.size(); ++i) {
      if (selector_.isReadable(server_sockets_[i].getFD())) {
        Client new_client = server_sockets_[i].accept();

        reserve_clients_.push_back(new_client);
      }
    }
  } catch (const std::exception& e) {
    // Error::log("acceptConnections() failed", e.what());
  }
}

void ServerHandler::respondToClients() {
  try {
    Client* client;
    std::vector<int> delete_clients;

    delete_clients.reserve(clients_.size());

    for (clients_type::iterator it = clients_.begin(); it != clients_.end();
         ++it) {
      client = &it->second;
      int client_fd = client->getFD();
      try {
        if (selector_.isReadable(client_fd)) {
          receiveRequest(*client);
        }
        if (client->isReadyToCgiIO()) {
          client->executeCgiIO(selector_);
        }
        if (selector_.isWritable(client_fd) && client->isReadyToSend()) {
          sendResponse(*client);
        }
      } catch (const Client::ConnectionClosedException& e) {
        delete_clients.push_back(client_fd);
        client->closeConnection();
      }
    }
    registerReserveClients();
    if (!delete_clients.empty()) {
      deleteClients(delete_clients);
    }
  } catch (const std::exception& e) {
    // Error::log("respondToClients() failed", e.what());
  }
}

void ServerHandler::handleTimeout() {
  deleteTimeoutClients();
  deleteTimeoutSessions();
}

void ServerHandler::receiveRequest(Client& client) {
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

      if (!client.hasCookie() || !isValidSessionID(client)) {
        response_obj.setSession(generateSession(client));
      } else {
        response_obj.setSession(findSession(client));
      }

      if (client.isCgi()) {
        Cgi& cgi = client.getCgi();
        cgi.runCgiScript(request_obj, response_obj, client.getClientAddress(),
                         client.getServerAddress(),
                         location_block.getCgiParam("CGI_PATH"));

        selector_.registerFD(cgi.getReadFD());
        if (request_obj.getMethod() == "POST") {
          selector_.registerFD(cgi.getWriteFD());
        }
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

void ServerHandler::sendResponse(Client& client) {
  HttpResponse& response_obj = client.getResponseObj();
  try {
    client.send();
    if (!client.isPartialWritten()) {
      const HttpRequest& request_obj = client.getRequestObj();
      if (request_obj.getHeader("CONNECTION") == "close" ||
          !response_obj.isSuccessCode()) {
        throw Client::ConnectionClosedException();
      }
      client.clear();
    }
    client.setTimeout();
    Session* session = response_obj.getSession();
    if (session) {
      session->setTimeout();
    }
  } catch (const Client::ConnectionClosedException& e) {
    throw Client::ConnectionClosedException();
  } catch (const std::exception& e) {
    response_obj.setStatus(C500);
  }
}

void ServerHandler::registerReserveClients() {
  for (std::size_t i = 0; i < reserve_clients_.size(); ++i) {
    Client reserve_client = reserve_clients_[i];
    int fd = reserve_client.getFD();

    selector_.registerFD(fd);
    clients_.insert(std::make_pair(fd, reserve_client));
  }
  reserve_clients_.clear();
}

const ServerBlock& ServerHandler::findServerBlock(
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

void ServerHandler::deleteTimeoutClients() {
  Client* client;
  std::vector<int> delete_clients;

  delete_clients.reserve(clients_.size());

  for (clients_type::iterator it = clients_.begin(); it != clients_.end();
       ++it) {
    client = &it->second;
    if (client->getTimeout() < std::time(0) && !client->isResponseWaiting()) {
      client->closeConnection();
      delete_clients.push_back(client->getFD());
    }
  }
  if (!delete_clients.empty()) {
    deleteClients(delete_clients);
  }
}

void ServerHandler::deleteTimeoutSessions() {
  Session* session;
  std::vector<const std::string*> delete_sessions;

  for (sessions_type::iterator sessions_it = sessions_.begin();
       sessions_it != sessions_.end(); ++sessions_it) {
    delete_sessions.reserve(sessions_it->second.size());

    for (sessions_mapped_type::iterator mapped_it = sessions_it->second.begin();
         mapped_it != sessions_it->second.end(); ++mapped_it) {
      session = &mapped_it->second;
      if (session->getTimeout() < std::time(NULL)) {
        delete_sessions.push_back(&mapped_it->first);
        std::string path = "rm -rf ./upload_file/" +
                           toString(sessions_it->first) + "/" +
                           session->getID();
        system(path.c_str());
      }
    }
    if (!delete_sessions.empty()) {
      deleteSessions(sessions_it->second, delete_sessions);
    }
    delete_sessions.clear();
  }
}

void ServerHandler::deleteClients(const std::vector<int>& delete_clients) {
  for (std::size_t i = 0; i < delete_clients.size(); ++i) {
    clients_.erase(delete_clients[i]);
    selector_.unregisterFD(delete_clients[i]);
  }
}

void ServerHandler::deleteSessions(
    sessions_mapped_type& dest,
    const std::vector<const std::string*>& delete_sessions) {
  for (std::size_t i = 0; i < delete_sessions.size(); ++i) {
    dest.erase(*delete_sessions[i]);
  }
}

Session* ServerHandler::generateSession(Client& client) {
  int server_block_key = client.getServerBlockKey();
  std::string session_id = generateSessionID(server_block_key);
  std::string session_key = session_id + ":" +
                            client.getClientAddress().getIP() + ":" +
                            client.getRequestObj().getHeader("USER-AGENT");

  sessions_[server_block_key].insert(
      std::make_pair(session_key, Session(session_id)));
  return &sessions_[server_block_key].find(session_key)->second;
}

bool ServerHandler::isDuplicatedId(int server_block_key,
                                   std::string session_id) {
  for (ServerHandler::sessions_mapped_type::iterator it =
           sessions_[server_block_key].begin();
       it != sessions_[server_block_key].end(); ++it) {
    if (it->second.getID() == session_id) {
      return true;
    }
  }
  return false;
}

std::string ServerHandler::generateSessionID(int server_block_key) {
  std::string session_id;

  do {
    session_id = toString(std::rand());
  } while (isDuplicatedId(server_block_key, session_id));
  return session_id;
}

bool ServerHandler::isValidSessionID(const Client& client) {
  int server_block_key = client.getServerBlockKey();
  std::string session_key = client.getSessionKey();

  try {
    sessions_[server_block_key].at(session_key);
  } catch (const std::out_of_range& e) {
    return false;
  }
  return true;
}

Session* ServerHandler::findSession(const Client& client) {
  int server_block_key = client.getServerBlockKey();
  std::string session_key = client.getSessionKey();

  return &sessions_[server_block_key].find(session_key)->second;
}

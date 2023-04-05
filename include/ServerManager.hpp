#ifndef SERVER_MANAGER_HPP_
#define SERVER_MANAGER_HPP_

#include <map>
#include <queue>
#include <string>
#include <vector>

#include "Client.hpp"
#include "Config.hpp"
#include "HttpRequest.hpp"
#include "LocationBlock.hpp"
#include "Selector.hpp"
#include "ServerBlock.hpp"
#include "ServerSocket.hpp"
#include "Session.hpp"
#include "SessionHandler.hpp"

class ServerManager {
 public:
  typedef std::map<std::string, std::vector<ServerBlock> > server_blocks_type;
  typedef std::vector<ServerSocket> server_sockets_type;
  typedef std::map<int, Client> clients_type;
  typedef Selector selector_type;
  typedef SessionHandler session_handler_type;

  ServerManager();
  ServerManager(const ServerManager& src);
  ~ServerManager();

  void registerSignalHandlers() const;
  void configureServer(const Config& config);
  void createServers();
  int select();
  void acceptConnections();
  void respondToClients();
  void handleTimeout();

 private:
  void receiveRequest(Client& client);
  void sendResponse(Client& client);
  const ServerBlock& findServerBlock(const std::string& server_socket_key,
                                     const std::string& server_name);
  void validateRequest(const HttpRequest& request_obj,
                       const LocationBlock& location_block);
  void deleteClients(std::queue<int>& delete_clients);
  void deleteTimeoutClients();

  server_blocks_type server_blocks_;
  server_sockets_type server_sockets_;
  clients_type clients_;
  selector_type selector_;
  session_handler_type session_handler_;
};

#endif

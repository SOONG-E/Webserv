#ifndef SERVER_HANDLER_HPP_
#define SERVER_HANDLER_HPP_

#include <map>
#include <string>
#include <vector>

#include "Client.hpp"
#include "Config.hpp"
#include "HttpRequest.hpp"
#include "LocationBlock.hpp"
#include "Selector.hpp"
#include "ServerBlock.hpp"
#include "ServerSocket.hpp"

class ServerHandler {
 public:
  ServerHandler();
  ServerHandler(const ServerHandler& src);
  ServerHandler& operator=(const ServerHandler& src);
  ~ServerHandler();

  void configureServer(const Config& config);
  void createServers();
  void acceptConnections();
  void respondToClients();

 private:
  typedef std::map<std::string, std::vector<ServerBlock> > server_blocks_type;
  typedef std::vector<ServerSocket> server_sockets_type;
  typedef std::map<int, Client> clients_type;
  typedef Selector selector_type;

  const ServerBlock& findServerBlock(const std::string& socket_key,
                                     const std::string& server_name);
  void receiveRequest(Client& client, std::vector<int>& delete_clients);
  void sendResponse(Client& client, std::vector<int>& delete_clients);
  void closeConnection(Client& client);
  void deleteClients(const std::vector<int>& delete_clients);
  void validateRequest(const HttpRequest& request_obj,
                       const LocationBlock& location_block);
  void generateResponse(Client& client);
  bool isCgi(const std::string& request_uri) const;

  server_blocks_type server_blocks_;
  server_sockets_type server_sockets_;
  clients_type clients_;
  selector_type server_selector_;
  selector_type client_selector_;
  selector_type cgi_selector_;
};

#endif

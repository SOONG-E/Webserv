#ifndef SERVER_HANDLER_HPP
#define SERVER_HANDLER_HPP

#include <list>
#include <map>
#include <vector>

#include "Client.hpp"
#include "Selector.hpp"
#include "ServerSocket.hpp"

class ServerHandler {
 public:
  ServerHandler();
  ServerHandler(const ServerHandler &src);
  ~ServerHandler();

  ServerHandler &operator=(const ServerHandler &src);

  void configureServer(const Config &config);
  void createServers();
  void acceptConnections();
  void respondToClients();

 private:
  typedef std::map<std::string, std::vector<ServerBlock> > server_blocks_type;
  typedef std::vector<ServerSocket> server_sockets_type;
  typedef std::map<int, Client> clients_type;
  typedef Selector selector_type;

  server_blocks_type _server_blocks;
  server_sockets_type _server_sockets;
  clients_type _clients;
  selector_type _server_selector;
  selector_type _client_selector;
};

#endif
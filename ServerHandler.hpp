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
  void createServerSockets();
  void acceptConnections();
  void respondToClients();

 private:
  typedef std::map<std::string, std::vector<ServerConfig> > server_config_type;
  typedef std::vector<ServerSocket> server_sockets_type;
  typedef std::list<Client> clients_type;
  typedef Selector selector_type;

  server_config_type _server_configs;
  server_sockets_type _server_sockets;
  clients_type _clients;
  selector_type _server_selector;
  selector_type _client_selector;
};

#endif
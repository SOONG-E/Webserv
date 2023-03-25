#ifndef SERVER_HANDLER_HPP
#define SERVER_HANDLER_HPP

#include <unistd.h>

#include <map>
#include <vector>

#include "Client.hpp"
#include "Config.hpp"
#include "HttpRequest.hpp"
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

  bool isImplementedMethod(std::string method) const;
  void validateRequest(Client *client);
  const ServerBlock *findServerBlock(const std::string &socket_key,
                                     const std::string &server_name);
  const LocationBlock *findLocationBlock(const ServerBlock *server_block,
                                         const std::string &request_uri);
  void receiveRequest(Client *client, std::vector<int> &delete_clients);
  void sendResponse(Client *client, std::vector<int> &delete_clients);
  void closeConnection(Client *client);
  void deleteClients(const std::vector<int> &delete_clients);

  server_blocks_type server_blocks_;
  server_sockets_type server_sockets_;
  clients_type clients_;
  selector_type server_selector_;
  selector_type client_selector_;
};

#endif
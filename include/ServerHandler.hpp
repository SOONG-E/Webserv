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
#include "Session.hpp"

class ServerHandler {
 public:
  ServerHandler(std::size_t server_block_count);
  ServerHandler(const ServerHandler& src);
  ~ServerHandler();

  void registerSignalHandlers() const;
  void configureServer(const Config& config);
  void createServers();
  void acceptConnections();
  void respondToClients();
  void handleTimeout();

 private:
  typedef std::map<std::string, std::vector<ServerBlock> > server_blocks_type;
  typedef std::vector<ServerSocket> server_sockets_type;
  typedef std::map<int, Client> clients_type;
  typedef Selector selector_type;
  typedef std::map<std::string, Session> sessions_mapped_type;
  typedef std::map<int, sessions_mapped_type> sessions_type;
  typedef std::vector<unsigned long long> avail_session_id_type;

  void receiveRequest(Client& client);
  void sendResponse(Client& client);
  const ServerBlock& findServerBlock(const std::string& server_socket_key,
                                     const std::string& server_name);
  void validateRequest(const HttpRequest& request_obj,
                       const LocationBlock& location_block);
  void deleteTimeoutClients();
  void deleteTimeoutSessions();
  void deleteClients(const std::vector<int>& delete_clients);
  void deleteSessions(sessions_mapped_type& dest,
                      const std::vector<const std::string*>& delete_sessions);
  void generateSession(Client& client);
  std::string generateSessionID(int server_block_key);
  bool isValidSessionID(const Client& client);
  bool isDuplicatedId(int server_block_key, std::string session_id);
  Session& findSession(const Client& client);

  server_blocks_type server_blocks_;
  server_sockets_type server_sockets_;
  clients_type clients_;
  selector_type server_selector_;
  selector_type client_selector_;
  sessions_type sessions_;
  avail_session_id_type avail_session_id_;
};

#endif

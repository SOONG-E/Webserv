#ifndef SERVER_MANAGER_HPP_
#define SERVER_MANAGER_HPP_

#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <exception>
#include <map>
#include <string>

#include "Client.hpp"
#include "Config.hpp"
#include "TcpServer.hpp"

class SocketAddress;
class Client;

class ServerManager {
 public:
  ServerManager(const Config &config);
  ~ServerManager();

  void setServer(void);
  void runServer(void);

  void createEvent(uintptr_t ident, int16_t filter, uint16_t flags,
                   uint32_t fflags, intptr_t data, void *udata);

 private:
  typedef std::map<std::string, TcpServer *> TcpServerType;
  typedef std::vector<HttpServer *> HttpServerType;
  typedef std::map<int, Client *> ClientType;

  void registerServer(const Config &config);
  TcpServer *getTcpServer(const std::string &key);
  TcpServer *createTcpServer(const std::string &key);
  HttpServer *createHttpServer(const ServerBlock &server_block);
  void createClient(const int client_fd, const TcpServer *tcp_server,
                    const SocketAddress socket_address);

  void bindServers(void);
  int createListenSocket(void) const;
  struct addrinfo *getAddrInfo(const std::string &ip, const std::string &port);

  void createListenEvent(int fd, TcpServer *server);

  void processEventOnQueue(const int events);
  void acceptNewClient(const int server_socker, const TcpServer *tcp_server);
  void unconnectClient(const int client_fd);

  /*member variables*/
  const int kq_;
  TcpServerType tcp_servers_;
  HttpServerType http_servers_;
  ClientType clients_;
  std::set<int> listen_sockets_;

  std::vector<struct kevent> change_event_list;
  struct kevent event_list[CAPABLE_EVENT_SIZE];
};

#endif
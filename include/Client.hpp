#ifndef CLIENT_HPP_
#define CLIENT_HPP_

#include <sys/event.h>

#include <ctime>
#include <exception>
#include <string>

#include "HttpRequest.hpp"
#include "SocketAddress.hpp"
#include "TcpServer.hpp"

class Client {
 public:
  Client(const int fd, const TcpServer* tcp_server,
         const SocketAddress& address);
  Client(const Client& origin);
  ~Client();

  int getFd(void) const;
  HttpServer* getHttpServer(void) const;
  HttpRequest getRequest(void) const;
  std::string getResponse(void) const;

  void processEvent(const int event_type);

  /* request */
  void processRequest(void);
  std::string readData(void);

  /* handler */
  void lookUpHttpServer(void);
  void lookUpLocation(void);
  void passRequestToHandler(void);

  /* response */
  void writeData(void);

 private:
  const int fd_;
  const TcpServer* tcp_server_;
  const SocketAddress address_;
  HttpServer* http_server_;
  Location location_;
  HttpRequest request_;
  std::string response_;
  std::string status_;
  bool is_response_ready;
};

#endif

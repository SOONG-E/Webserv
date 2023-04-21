#ifndef CLIENT_HPP_
#define CLIENT_HPP_

#include <sys/event.h>

#include <ctime>
#include <exception>
#include <string>

#include "HttpRequest.hpp"
#include "Response.hpp"
#include "ResponseGenerator.hpp"
#include "ServerManager.hpp"
#include "SocketAddress.hpp"
#include "StaticContentHandler.hpp"
#include "TcpServer.hpp"
#include "exception.hpp"

class HttpServer;
class ServerManager;
class TcpServer;

class Client {
 public:
  Client(const int fd, const TcpServer* tcp_server,
         const SocketAddress& address, ServerManager* manager);
  Client(const Client& origin);
  ~Client();

  int getFd(void) const;
  HttpServer* getHttpServer(void) const;
  Location& getLocation(void);
  const Location& getLocation(void) const;
  HttpRequest& getRequest(void);
  const HttpRequest& getRequest(void) const;
  std::string& getResponse(void);
  const std::string& getResponse(void) const;
  int& getStatus(void);
  const int& getStatus(void) const;
  std::string& getFullUri(void);
  const std::string& getFullUri(void) const;

  void setStatus(int status);

  void processEvent(const int event_type);

  /* request */
  void processRequest(void);
  std::string readData(void);

  /* handler */
  void lookUpHttpServer(void);
  void lookUpLocation(void);
  void setFullUri(void);
  void passErrorToHandler(int status);
  void passRequestToHandler(void);
  void setToSend(bool set);

  /* response */
  void writeData(void);

  bool isErrorCode(void);
  void clearClient(void);
  void unconnectClient(void);

 private:
  ServerManager* manager_;
  const int fd_;
  const TcpServer* tcp_server_;
  const SocketAddress address_;
  HttpServer* http_server_;
  Location location_;
  HttpRequest request_;
  std::string fullUri_;
  std::string response_;
  int status_;
  bool is_response_ready_;
};

#endif

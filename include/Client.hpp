#ifndef CLIENT_HPP_
#define CLIENT_HPP_

#include <sys/event.h>

#include <algorithm>
#include <ctime>
#include <exception>
#include <string>

#include "HttpRequest.hpp"
#include "Response.hpp"
#include "ResponseGenerator.hpp"
#include "ServerManager.hpp"
#include "SocketAddress.hpp"
#include "TcpServer.hpp"
#include "exception.hpp"
#include "handler.hpp"

class HttpServer;
class ServerManager;
class TcpServer;

class Client {
 public:
  Client(const int fd, const TcpServer* tcp_server,
         const SocketAddress& address, ServerManager* manager);
  Client(const Client& origin);
  Client operator=(const Client& origin);
  ~Client();

  ServerManager* getServerManager(void);
  int getFd(void) const;
  const TcpServer* getTcpServer(void) const;
  HttpServer* getHttpServer(void) const;
  const SocketAddress getAddr(void) const;
  Location& getLocation(void);
  const Location& getLocation(void) const;
  HttpRequest& getRequest(void);
  const HttpRequest& getRequest(void) const;
  Process& getProcess(void);
  std::string& getResponse(void);
  const std::string& getResponse(void) const;
  int& getStatus(void);
  const int& getStatus(void) const;
  std::string& getFullUri(void);
  const std::string& getFullUri(void) const;

  void setStatus(int status);
  void setSession(Session* session);
  void setProcess(Process& cgi_process);

  void setClientTimeout(std::time_t time = std::time(NULL));
  void setSessionTimeout(void);
  void setAllTimeout(std::time_t time = std::time(NULL));
  void setTimer(std::time_t time = std::time(NULL));
  void handleTimeout(void);

  void processEvent(const int event_type);

  /* request */
  void processRequest(void);
  std::string readData(void);
  void lookUpHttpServer(void);
  void lookUpLocation(void);
  void setFullUri(void);
  void setSession(void);
  void validAuth(void);

  /* handler */
  void passErrorToHandler(int status);
  void passRequestToHandler(void);
  void passToCgi(const int event_type);

  /* response */
  void writeData(void);

  void setToSend(bool set);
  bool isErrorCode(void);
  bool isCgiStarted(void);
  bool isCgiDone(void);
  void clear(void);

 private:
  ServerManager* manager_;
  const int fd_;
  Session* session_;
  const TcpServer* tcp_server_;
  const SocketAddress address_;
  HttpServer* http_server_;
  Location location_;
  HttpRequest request_;
  Process cgi_process_;
  std::string fullUri_;
  std::string response_;
  int status_;
  std::time_t timeout_;

  bool is_response_ready_;
};

#endif

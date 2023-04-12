#ifndef CLIENT_HPP_
#define CLIENT_HPP_

#include <ctime>
#include <exception>
#include <string>

#include "Cgi.hpp"
#include "HttpParser.hpp"
#include "HttpResponse.hpp"
#include "Selector.hpp"
#include "ServerBlock.hpp"
#include "Session.hpp"
#include "SocketAddress.hpp"

class Client {
 public:
  Client(int fd, const ServerBlock& default_server,
         const SocketAddress& cli_addr, const SocketAddress& serv_addr);
  Client(const Client& src);
  ~Client();

  int getFD() const;
  std::string getServerSocketKey() const;
  int getServerBlockKey() const;
  std::string getSessionKey() const;
  HttpRequest& getRequest();
  const HttpRequest& getRequest() const;
  HttpResponse& getResponse();
  const HttpResponse& getResponse() const;
  Cgi& getCgi();
  const Cgi& getCgi() const;
  const SocketAddress& getServerAddress() const;
  const SocketAddress& getClientAddress() const;
  std::time_t getTimeout() const;

  void setTimeout(std::time_t time = std::time(NULL));
  void setSessionTimeout();

  std::string receive() const;
  void send();
  void runCgiProcess(Selector& selector);
  void executeCgiIO(Selector& selector);
  void closeConnection() const;
  void clear();

  bool isCgi() const;
  bool isPartialResponse() const;
  bool isReadyToCgiIO() const;
  bool isReadyToSend() const;

  void logAddressInfo() const;
  void logConnectionInfo() const;
  void logReceiveInfo(const std::string& request) const;

  class ConnectionClosedException : public std::exception {
   public:
    const char* what() const throw();
  };

 private:
  int fd_;
  SocketAddress cli_address_;
  SocketAddress serv_address_;
  HttpRequest request_;
  Cgi cgi_;
  std::string buf_;
  std::time_t timeout_;
};

#endif

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
#include "SocketAddress.hpp"
#include "exception.hpp"

class Client {
 public:
  Client(int fd, const ServerBlock& default_server,
         const SocketAddress& cli_addr, const SocketAddress& serv_addr);
  Client(const Client& src);
  ~Client();

  int getFD() const;
  HttpParser& getParser();
  const HttpParser& getParser() const;
  std::string getServerKey() const;
  std::string getClientKey() const;
  const HttpRequest& getRequestObj() const;
  HttpResponse& getResponseObj();
  const HttpResponse& getResponseObj() const;
  Cgi& getCgi();
  const Cgi& getCgi() const;
  const SocketAddress& getServerAddress() const;
  const SocketAddress& getClientAddress() const;
  std::time_t getTimeout() const;
  void setTimeout(std::time_t time = std::time(NULL));
  void setSessionId(std::string session_id);

  std::string receive() const;
  void send();
  void executeCgiIO();
  void closeConnection() const;
  void clear();

  bool isCgi() const;
  bool isPartialWritten() const;
  bool isReadyToCgiIO() const;
  bool isReadyToSend() const;
  bool isReceiveFinished() const;
  bool hasCookie() const;

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
  HttpParser parser_;
  HttpResponse response_obj_;
  Cgi cgi_;
  std::string buf_;
  std::time_t timeout_;
};

#endif

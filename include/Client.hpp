#ifndef CLIENT_HPP_
#define CLIENT_HPP_

#include <exception>
#include <string>

#include "Cgi.hpp"
#include "HttpParser.hpp"
#include "HttpResponse.hpp"
#include "Selector.hpp"
#include "ServerBlock.hpp"
#include "SocketAddress.hpp"

class Client {
 public:
  Client(int fd, const ServerBlock& default_server,
         const SocketAddress& cli_addr, const SocketAddress& serv_addr);
  Client(const Client& src);
  ~Client();

  int getFD() const;
  HttpParser& getParser();
  const HttpParser& getParser() const;
  std::string getSocketKey() const;
  const HttpRequest& getRequestObj() const;
  HttpResponse& getResponseObj();
  const HttpResponse& getResponseObj() const;
  Cgi& getCgi();
  const Cgi& getCgi() const;
  const SocketAddress& getServerAddress() const;
  const SocketAddress& getClientAddress() const;

  std::string receive() const;
  void send();

  void executeCgiIO();

  void clearBuffer();
  void clear();

  bool isPartialWritten() const;
  bool isReadyToSend() const;

  void logAddressInfo() const;
  void logConnectionInfo() const;
  void logReceiveInfo(const std::string& request) const;

 private:
  int fd_;
  SocketAddress cli_address_;
  SocketAddress serv_address_;
  HttpParser parser_;
  HttpResponse response_obj_;
  Cgi cgi_;
  std::string buf_;

 public:
  class SocketReceiveException : public std::exception {
   public:
    SocketReceiveException(const char* cause);
    const char* what() const throw();

   private:
    const char* cause;
  };

  class SocketSendException : public std::exception {
   public:
    SocketSendException(const char* cause);
    const char* what() const throw();

   private:
    const char* cause;
  };
};

#endif

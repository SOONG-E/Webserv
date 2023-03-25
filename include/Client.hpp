#ifndef CLIENT_HPP
#define CLIENT_HPP

#define BUF_SIZE 65536

#include <cerrno>
#include <cstring>
#include <iostream>
#include <string>

#include "HttpParser.hpp"
#include "HttpResponse.hpp"
#include "Log.hpp"
#include "ServerBlock.hpp"
#include "SocketAddress.hpp"

class Client {
 public:
  Client(int fd, const SocketAddress& cli_addr, const SocketAddress& serv_addr);
  Client(const Client& src);
  ~Client();

  Client& operator=(const Client& src);

  int getFD() const;
  HttpParser& getParser();
  const HttpParser& getParser() const;
  std::string getSocketKey() const;
  std::size_t getClientMaxBodySize() const;
  std::string getRequestMethod() const;
  std::size_t getRequestContentLength() const;
  std::string getRequestHeader(const std::string& target) const;
  const ServerBlock* getResponseServerBlock() const;
  const LocationBlock* getResponseLocationBlock() const;
  const std::string& getRequestUri() const;
  void setResponseServerBlock(const ServerBlock* server_block);
  void setResponseLocationBlock(const LocationBlock* location_block);

  void setResponseStatus(const std::string& code, const std::string& reason);
  void setServerBlock(const ServerBlock* server_block);
  void setLocationBlock(const LocationBlock* location_block);
  void clearParser();
  void clearResponseBuf();
  void appendRequest(const std::string& request);

  std::string receive() const;
  void send();

  bool isAllowedMethod() const;
  bool isErrorStatus() const;
  bool isPartialWritten() const;
  bool isParseCompleted() const;

  void logAddressInfo() const;
  void logConnectionInfo() const;
  void logReceiveInfo(const std::string& request) const;

 private:
  int fd_;
  SocketAddress cli_address_;
  SocketAddress serv_address_;
  HttpParser parser_;
  HttpResponse response_obj_;
  std::string response_buf_;

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
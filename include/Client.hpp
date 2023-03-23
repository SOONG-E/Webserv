#ifndef CLIENT_HPP
#define CLIENT_HPP

#define BUF_SIZE 65536

#include <cerrno>
#include <cstring>
#include <string>

#include "HttpParser.hpp"
#include "HttpResponse.hpp"
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
  const std::string& getSocketKey() const;
  HttpResponse& getResponseObj();
  const HttpResponse& getResponseObj() const;
  std::string getRequestMethod() const;
  std::string getRequestHeader(const std::string& target) const;

  void setResponseStatus(const std::string& code, const std::string& reason);
  void clearParser();
  void clearResponseBuf();
  void appendRequest(const std::string& request);

  std::string receive() const;
  void send(const ServerBlock* server_block);

  bool isResponseSuccess() const;
  bool isPartialWritten() const;
  bool isParseCompleted() const;

 private:
  int fd_;
  std::string socket_key_;
  SocketAddress cli_address_;
  SocketAddress serv_address_;
  HttpParser parser_;
  HttpResponse response_obj_;
  std::string response_buf_;

 public:
  class SocketReceiveException : public std::exception {
   public:
    SocketReceiveException(const char* cause);

   private:
    const char* cause;

    const char* what() const throw();
  };

  class SocketSendException : public std::exception {
   public:
    SocketSendException(const char* cause);

   private:
    const char* cause;

    const char* what() const throw();
  };
};

#endif
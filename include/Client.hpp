#ifndef CLIENT_HPP
#define CLIENT_HPP

#define BUF_SIZE 65536

#include <cstring>
#include <string>

#include "HttpParser.hpp"
#include "HttpResponse.hpp"
#include "ServerBlock.hpp"
#include "SocketAddress.hpp"

class Client {
 public:
  Client(int socket, const SocketAddress& cli_addr,
         const SocketAddress& serv_addr);
  Client(const Client& src);
  ~Client();

  Client& operator=(const Client& src);

  int getSocket() const;
  HttpParser& getParser();
  const HttpParser& getParser() const;
  const std::string& getSocketKey() const;
  HttpResponse& getResponseObj();
  const HttpResponse& getResponseObj() const;

  std::string receive() const;
  void send(const ServerBlock* server_block);

  bool isPartialWritten() const;

 private:
  int socket_;
  std::string socket_key_;
  SocketAddress cli_address_;
  SocketAddress serv_address_;
  HttpParser parser_;
  HttpResponse response_obj_;

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
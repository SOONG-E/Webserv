#ifndef CLIENT_HPP
#define CLIENT_HPP

#define BUF_SIZE 65536

#include <string>

#include "HttpParser.hpp"
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
  std::string getKey() const;
  void setServerBlock(const ServerBlock* server_block);
  std::string receive() const;

 private:
  int socket_;
  SocketAddress cli_addr_;
  SocketAddress serv_addr_;
  HttpParser parser_;
  const ServerBlock* server_block_;

  // exception
  class SocketReceiveException : public std::exception {
   public:
    SocketReceiveException(const char* cause);

   private:
    const char* cause;

    const char* what() const throw();
  };
};

#endif
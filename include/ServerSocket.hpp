#ifndef SERVER_SOCKET_HPP_
#define SERVER_SOCKET_HPP_

#include <exception>

#include "Client.hpp"
#include "ServerBlock.hpp"
#include "SocketAddress.hpp"

class ServerSocket {
 public:
  ServerSocket();
  ServerSocket(const ServerSocket& src);
  ServerSocket& operator=(const ServerSocket& src);
  ~ServerSocket();

  void open();
  void bind(const SocketAddress& address, int backlog);
  Client accept(const ServerBlock& default_server) const;
  int getFD() const;
  const SocketAddress& getAddress() const;

 private:
  int fd_;
  SocketAddress address_;

 public:
  class SocketOpenException : public std::exception {
   public:
    SocketOpenException(const char* cause);
    const char* what() const throw();

   private:
    const char* cause;
  };

  class SocketBindException : public std::exception {
   public:
    SocketBindException(const char* cause);
    const char* what() const throw();

   private:
    const char* cause;
  };

  class SocketAcceptException : public std::exception {
   public:
    SocketAcceptException(const char* cause);
    const char* what() const throw();

   private:
    const char* cause;
  };
};

#endif

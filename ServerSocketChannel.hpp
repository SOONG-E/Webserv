#ifndef SOCKET_CHANNEL_HPP
#define SOCKET_CHANNEL_HPP

#include <fcntl.h>
#include <sys/socket.h>

#include <set>

#include "ClientSocketChannel.hpp"
#include "InetSocketAddress.hpp"

class ServerSocketChannel {
 public:
  ServerSocketChannel();
  ServerSocketChannel(const ServerSocketChannel& src);
  virtual ~ServerSocketChannel();

  ServerSocketChannel& operator=(const ServerSocketChannel& src);

  void bind(const InetSocketAddress& addr_info, int backlog);
  ClientSocketChannel accept();
  void configureNonBlock();
  int getSocket() const;

 private:
  int socket_fd;
  InetSocketAddress server_address;

 public:
  class SocketOpenException : public std::exception {
   public:
    SocketOpenException(const char* cause);

   private:
    const char* cause;

    const char* what() const throw();
  };

  class SocketBindException : public std::exception {
   public:
    SocketBindException(const char* cause);

   private:
    const char* cause;

    const char* what() const throw();
  };

  class SocketSetFlagException : public std::exception {
   public:
    SocketSetFlagException(const char* cause);

   private:
    const char* cause;

    const char* what() const throw();
  };

  class SocketAcceptException : public std::exception {
   public:
    SocketAcceptException(const char* cause);

   private:
    const char* cause;

    const char* what() const throw();
  };
};

#endif
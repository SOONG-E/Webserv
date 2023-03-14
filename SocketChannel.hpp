#ifndef SOCKET_CHANNEL_HPP
#define SOCKET_CHANNEL_HPP

#include <fcntl.h>
#include <sys/socket.h>

#include <set>

#include "InetSocketAddress.hpp"

class SocketChannel {
 public:
  SocketChannel();
  SocketChannel(const SocketChannel& src);
  virtual ~SocketChannel();

  SocketChannel& operator=(const SocketChannel& src);

  void bind(const InetSocketAddress& addr_info, int backlog);
  void accept();
  void configureNonBlock();
  int getListenSocket() const;
  std::set<int>::const_iterator getClientSockets() const;
  int removeClientSocket(int val);

 private:
  int listen_socket;
  std::set<int> client_sockets;

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
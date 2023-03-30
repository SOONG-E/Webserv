#ifndef SERVER_SOCKET_HPP_
#define SERVER_SOCKET_HPP_

#include <exception>

#include "Client.hpp"
#include "ServerBlock.hpp"
#include "SocketAddress.hpp"

class ServerSocket {
 public:
  ServerSocket(const ServerBlock& default_server);
  ServerSocket(const ServerSocket& src);
  ~ServerSocket();

  void open();
  void bind(const SocketAddress& address, int backlog);
  Client accept() const;
  int getFD() const;

 private:
  int fd_;
  const ServerBlock& default_server_;
  SocketAddress address_;
};

#endif

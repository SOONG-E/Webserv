#ifndef CLIENT_SOCKET_CHANNEL_HPP
#define CLIENT_SOCKET_CHANNEL_HPP

#include "InetSocketAddress.hpp"

class ClientSocketChannel {
 public:
  ClientSocketChannel(int socket_fd, const InetSocketAddress& server_address,
                      const InetSocketAddress& client_address);
  ClientSocketChannel(const ClientSocketChannel& src);
  ~ClientSocketChannel();

  ClientSocketChannel& operator=(const ClientSocketChannel& src);

  int getSocket() const;

 private:
  int socket_fd;
  InetSocketAddress server_address;
  InetSocketAddress client_address;
};

#endif
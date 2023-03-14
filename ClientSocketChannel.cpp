#include "ClientSocketChannel.hpp"

ClientSocketChannel::ClientSocketChannel(
    int socket_fd, const InetSocketAddress& server_address,
    const InetSocketAddress& client_address)
    : socket_fd(socket_fd),
      server_address(server_address),
      client_address(client_address) {}
ClientSocketChannel::ClientSocketChannel(const ClientSocketChannel& src) {
  *this = src;
}
ClientSocketChannel::~ClientSocketChannel() {}

ClientSocketChannel& ClientSocketChannel::operator=(
    const ClientSocketChannel& src) {
  socket_fd = src.socket_fd;
  server_address = src.server_address;
  client_address = src.client_address;
}

int ClientSocketChannel::getSocket() const { return socket_fd; }
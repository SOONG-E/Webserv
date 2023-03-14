#include "ServerSocketChannel.hpp"

ServerSocketChannel::ServerSocketChannel() {
  socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_fd == -1) throw SocketOpenException(strerror(errno));
}

ServerSocketChannel::ServerSocketChannel(const ServerSocketChannel& src) {
  *this = src;
}

ServerSocketChannel::~ServerSocketChannel() {}

ServerSocketChannel& ServerSocketChannel::operator=(
    const ServerSocketChannel& src) {
  socket_fd = src.socket_fd;
}

void ServerSocketChannel::bind(const InetSocketAddress& addr_info,
                               int backlog) {
  const int enable = 1;
  if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    throw SocketBindException(strerror(errno));

  if (::bind(socket_fd, (sockaddr*)&addr_info.getAddress(),
             addr_info.getAddressLen()) == -1)
    throw SocketBindException(strerror(errno));

  if (listen(socket_fd, backlog) == -1)
    throw SocketBindException(strerror(errno));

  server_address = addr_info;
}

ClientSocketChannel ServerSocketChannel::accept() {
  sockaddr_in client_addr;
  socklen_t client_addrlen;

  int client_socket =
      ::accept(socket_fd, (sockaddr*)&client_addr, &client_addrlen);
  if (client_socket == -1) throw SocketAcceptException(strerror(errno));

  return ClientSocketChannel(client_socket,
                             InetSocketAddress(server_address.getAddress(),
                                               server_address.getAddressLen()),
                             InetSocketAddress(client_addr, client_addrlen));
}

void ServerSocketChannel::configureNonBlock() {
  if (fcntl(socket_fd, F_SETFL, O_NONBLOCK) == -1)
    throw SocketSetFlagException(strerror(errno));
}

int ServerSocketChannel::getSocket() const { return socket_fd; }

// exception
ServerSocketChannel::SocketOpenException::SocketOpenException(const char* cause)
    : cause(cause) {}

const char* ServerSocketChannel::SocketOpenException::what() const throw() {
  return cause;
}

ServerSocketChannel::SocketBindException::SocketBindException(const char* cause)
    : cause(cause) {}

const char* ServerSocketChannel::SocketBindException::what() const throw() {
  return cause;
}

ServerSocketChannel::SocketSetFlagException::SocketSetFlagException(
    const char* cause)
    : cause(cause) {}

const char* ServerSocketChannel::SocketSetFlagException::what() const throw() {
  return cause;
}

ServerSocketChannel::SocketAcceptException::SocketAcceptException(
    const char* cause)
    : cause(cause) {}

const char* ServerSocketChannel::SocketAcceptException::what() const throw() {
  return cause;
}
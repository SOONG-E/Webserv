#include "ServerSocket.hpp"

ServerSocket::ServerSocket() : socket_(-1) {}

ServerSocket::ServerSocket(const ServerSocket& src) { *this = src; }

ServerSocket::~ServerSocket() {}

ServerSocket& ServerSocket::operator=(const ServerSocket& src) {
  socket_ = src.socket_;
  address_ = src.address_;

  return *this;
}

void ServerSocket::open() {
  if (socket_ != -1) throw SocketOpenException("already open");
  socket_ = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_ == -1) throw SocketOpenException(strerror(errno));
}

void ServerSocket::bind(const SocketAddress& address, int backlog) {
  const int enable = 1;
  if (setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    throw SocketBindException(strerror(errno));

  if (::bind(socket_, (sockaddr*)&address.getAddress(),
             address.getAddressLen()) == -1)
    throw SocketBindException(strerror(errno));

  if (fcntl(socket_, F_SETFL, O_NONBLOCK) == -1)
    throw SocketSetFlagException(strerror(errno));

  if (listen(socket_, backlog) == -1)
    throw SocketBindException(strerror(errno));

  address_ = address;
}

Client ServerSocket::accept() const {
  sockaddr client_addr;
  socklen_t client_addrlen;

  int client_socket = ::accept(socket_, &client_addr, &client_addrlen);
  if (client_socket == -1) throw SocketAcceptException(strerror(errno));

  if (fcntl(client_socket, F_SETFL, O_NONBLOCK) == -1)
    throw SocketSetFlagException(strerror(errno));

  return Client(client_socket, SocketAddress(client_addr, client_addrlen),
                address_);
}

int ServerSocket::getSocket() const { return socket_; }

const SocketAddress& ServerSocket::getAddress() const { return address_; }

// exception
ServerSocket::SocketOpenException::SocketOpenException(const char* cause)
    : cause(cause) {}

const char* ServerSocket::SocketOpenException::what() const throw() {
  return cause;
}

ServerSocket::SocketBindException::SocketBindException(const char* cause)
    : cause(cause) {}

const char* ServerSocket::SocketBindException::what() const throw() {
  return cause;
}

ServerSocket::SocketSetFlagException::SocketSetFlagException(const char* cause)
    : cause(cause) {}

const char* ServerSocket::SocketSetFlagException::what() const throw() {
  return cause;
}

ServerSocket::SocketAcceptException::SocketAcceptException(const char* cause)
    : cause(cause) {}

const char* ServerSocket::SocketAcceptException::what() const throw() {
  return cause;
}
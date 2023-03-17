#include "ServerSocket.hpp"

ServerSocket::ServerSocket() : _socket(-1) {}

ServerSocket::ServerSocket(const ServerSocket& src) { *this = src; }

ServerSocket::~ServerSocket() {}

ServerSocket& ServerSocket::operator=(const ServerSocket& src) {
  _socket = src._socket;
  _address = src._address;

  return *this;
}

void ServerSocket::open() {
  if (_socket != -1) throw SocketOpenException("already open");
  _socket = socket(AF_INET, SOCK_STREAM, 0);
  if (_socket == -1) throw SocketOpenException(strerror(errno));
}

void ServerSocket::bind(const InetSocketAddress& address, int backlog) {
  const int enable = 1;
  if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    throw SocketBindException(strerror(errno));

  if (::bind(_socket, (sockaddr*)&address.getAddress(),
             address.getAddressLen()) == -1)
    throw SocketBindException(strerror(errno));

  if (fcntl(_socket, F_SETFL, O_NONBLOCK) == -1)
    throw SocketSetFlagException(strerror(errno));

  if (listen(_socket, backlog) == -1)
    throw SocketBindException(strerror(errno));

  _address = address;
}

Client ServerSocket::accept() const {
  sockaddr client_addr;
  socklen_t client_addrlen;

  int client_socket = ::accept(_socket, &client_addr, &client_addrlen);
  if (client_socket == -1) throw SocketAcceptException(strerror(errno));

  if (fcntl(client_socket, F_SETFL, O_NONBLOCK) == -1)
    throw SocketSetFlagException(strerror(errno));

  return Client(client_socket, InetSocketAddress(client_addr, client_addrlen));
}

int ServerSocket::getSocket() const { return _socket; }

const InetSocketAddress& ServerSocket::getAddress() const { return _address; }

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
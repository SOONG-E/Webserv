#include "ServerSocket.hpp"

ServerSocket::ServerSocket() : _socket(-1) {}

ServerSocket::ServerSocket(const ServerSocket& src) { *this = src; }

ServerSocket::~ServerSocket() {}

ServerSocket& ServerSocket::operator=(const ServerSocket& src) {
  _socket = src._socket;
  _ip = src._ip;
  _port = src._port;
}

void ServerSocket::open() {
  if (_socket != -1) throw SocketOpenException("already open");
  _socket = socket(AF_INET, SOCK_STREAM, 0);
  if (_socket == -1) throw SocketOpenException(strerror(errno));
}

void ServerSocket::bind(const InetSocketAddress& addr_info, int backlog) {
  const int enable = 1;
  if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    throw SocketBindException(strerror(errno));

  if (::bind(_socket, (sockaddr*)&addr_info.getAddress(),
             addr_info.getAddressLen()) == -1)
    throw SocketBindException(strerror(errno));

  if (fcntl(_socket, F_SETFL, O_NONBLOCK) == -1)
    throw SocketSetFlagException(strerror(errno));

  if (listen(_socket, backlog) == -1)
    throw SocketBindException(strerror(errno));

  _ip = addr_info.getIP();
  _port = addr_info.getPort();
}

Client ServerSocket::accept() {
  sockaddr client_addr;
  socklen_t client_addrlen;

  int client_socket = ::accept(_socket, &client_addr, &client_addrlen);
  if (client_socket == -1) throw SocketAcceptException(strerror(errno));

  if (fcntl(client_socket, F_SETFL, O_NONBLOCK) == -1)
    throw SocketSetFlagException(strerror(errno));

  return Client(client_socket, InetSocketAddress(_ip, _port));
}

int ServerSocket::getSocket() const { return _socket; }

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
#include "SocketChannel.hpp"

SocketChannel::SocketChannel() {
  listen_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (listen_socket == -1) throw SocketOpenException(strerror(errno));
}

SocketChannel::SocketChannel(const SocketChannel& src) { *this = src; }

SocketChannel::~SocketChannel() {}

SocketChannel& SocketChannel::operator=(const SocketChannel& src) {
  listen_socket = src.listen_socket;
}

void SocketChannel::bind(const InetSocketAddress& addr_info) {
  bind(addr_info, 128);
}

void SocketChannel::bind(const InetSocketAddress& addr_info, int backlog) {
  const int enable = 1;
  if (setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &enable,
                 sizeof(int)) < 0)
    throw SocketBindException(strerror(errno));
  if (::bind(listen_socket, &addr_info.getAddr(), addr_info.getAddrLen()) == -1)
    throw SocketBindException(strerror(errno));
  if (listen(listen_socket, backlog) == -1)
    throw SocketBindException(strerror(errno));
}

void SocketChannel::accept() {
  sockaddr client_addr;
  socklen_t client_addrlen;
  int client_socket = ::accept(listen_socket, &client_addr, &client_addrlen);
  if (client_socket == -1) throw SocketAcceptException(strerror(errno));
  client_sockets.insert(client_socket);
}

void SocketChannel::configureNonBlock() {
  if (fcntl(listen_socket, F_SETFL, O_NONBLOCK) == -1)
    throw SocketSetFlagException(strerror(errno));
}

int SocketChannel::getListenSocket() const { return listen_socket; }

std::set<int>::const_iterator SocketChannel::getClientSockets() const {
  return client_sockets.begin();
}

int SocketChannel::removeClientSockets(int val) {
  return client_sockets.erase(val);
}

// exception
SocketChannel::SocketOpenException::SocketOpenException(const char* cause)
    : cause(cause) {}

const char* SocketChannel::SocketOpenException::what() const throw() {
  return cause;
}

SocketChannel::SocketBindException::SocketBindException(const char* cause)
    : cause(cause) {}

const char* SocketChannel::SocketBindException::what() const throw() {
  return cause;
}

SocketChannel::SocketSetFlagException::SocketSetFlagException(const char* cause)
    : cause(cause) {}

const char* SocketChannel::SocketSetFlagException::what() const throw() {
  return cause;
}

SocketChannel::SocketAcceptException::SocketAcceptException(const char* cause)
    : cause(cause) {}

const char* SocketChannel::SocketAcceptException::what() const throw() {
  return cause;
}
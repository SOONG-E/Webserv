#include "ServerSocket.hpp"

#include <fcntl.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>

#include "constant.hpp"

ServerSocket::ServerSocket() : fd_(-1) {}

ServerSocket::ServerSocket(const ServerSocket& src) { *this = src; }

ServerSocket& ServerSocket::operator=(const ServerSocket& src) {
  fd_ = src.fd_;
  address_ = src.address_;
  return *this;
}

ServerSocket::~ServerSocket() {}

void ServerSocket::open() {
  if (fd_ != -1) {
    throw SocketOpenException("already open");
  }
  fd_ = socket(AF_INET, SOCK_STREAM, 0);
  if (fd_ == ERROR<int>()) {
    throw SocketOpenException(strerror(errno));
  }
} 

void ServerSocket::bind(const SocketAddress& address, int backlog) {
  const int enable = 1;
  if (setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
    throw SocketBindException(strerror(errno));
  }

  if (::bind(fd_, (sockaddr*)&address.getAddress(), address.getAddressLen()) ==
      ERROR<int>()) {
    throw SocketBindException(strerror(errno));
  }

  if (fcntl(fd_, F_SETFL, O_NONBLOCK) == ERROR<int>()) {
    throw SocketBindException(strerror(errno));
  }

  if (listen(fd_, backlog) == ERROR<int>()) {
    throw SocketBindException(strerror(errno));
  }
  address_ = address;
}

Client ServerSocket::accept(const ServerBlock& default_server) const {
  sockaddr client_addr;
  socklen_t client_addrlen;

  int client_fd = ::accept(fd_, &client_addr, &client_addrlen);
  if (client_fd == ERROR<int>()) {
    throw SocketAcceptException(strerror(errno));
  }

  if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == ERROR<int>()) {
    close(client_fd);
    throw SocketAcceptException(strerror(errno));
  }

  return Client(client_fd, default_server,
                SocketAddress(client_addr, client_addrlen), address_);
}

int ServerSocket::getFD() const { return fd_; }

const SocketAddress& ServerSocket::getAddress() const { return address_; }

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

ServerSocket::SocketAcceptException::SocketAcceptException(const char* cause)
    : cause(cause) {}

const char* ServerSocket::SocketAcceptException::what() const throw() {
  return cause;
}

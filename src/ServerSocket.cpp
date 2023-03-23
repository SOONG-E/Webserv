#include "ServerSocket.hpp"

ServerSocket::ServerSocket() : fd_(-1) {}

ServerSocket::ServerSocket(const ServerSocket& src) { *this = src; }

ServerSocket::~ServerSocket() {}

ServerSocket& ServerSocket::operator=(const ServerSocket& src) {
  fd_ = src.fd_;
  address_ = src.address_;

  return *this;
}

void ServerSocket::open() {
  if (fd_ != -1) throw SocketOpenException("already open");
  fd_ = socket(AF_INET, SOCK_STREAM, 0);
  if (fd_ == -1) throw SocketOpenException(strerror(errno));
}

void ServerSocket::bind(const SocketAddress& address, int backlog) {
  const int enable = 1;
  if (setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    throw SocketBindException(strerror(errno));

  if (::bind(fd_, (sockaddr*)&address.getAddress(), address.getAddressLen()) ==
      -1)
    throw SocketBindException(strerror(errno));

  if (fcntl(fd_, F_SETFL, O_NONBLOCK) == -1)
    throw SocketBindException(strerror(errno));

  if (listen(fd_, backlog) == -1) throw SocketBindException(strerror(errno));

  address_ = address;
}

Client ServerSocket::accept() const {
  sockaddr client_addr;
  socklen_t client_addrlen;

  int client_fd = ::accept(fd_, &client_addr, &client_addrlen);
  if (client_fd == -1) {
    throw SocketAcceptException(strerror(errno));
  }

  if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1) {
    close(client_fd);
    throw SocketAcceptException(strerror(errno));
  }

  return Client(client_fd, SocketAddress(client_addr, client_addrlen),
                address_);
}

int ServerSocket::getFD() const { return fd_; }

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

ServerSocket::SocketAcceptException::SocketAcceptException(const char* cause)
    : cause(cause) {}

const char* ServerSocket::SocketAcceptException::what() const throw() {
  return cause;
}
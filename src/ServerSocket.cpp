#include "ServerSocket.hpp"

#include <fcntl.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>

#include "constant.hpp"

ServerSocket::ServerSocket(const ServerBlock& default_server)
    : fd_(-1), default_server_(default_server) {}

ServerSocket::ServerSocket(const ServerSocket& src)
    : fd_(src.fd_),
      default_server_(src.default_server_),
      address_(src.address_) {}

ServerSocket::~ServerSocket() {}

void ServerSocket::open() {
  if (fd_ != -1) {
    throw std::logic_error("already open");
  }
  fd_ = socket(AF_INET, SOCK_STREAM, 0);
  if (fd_ == ERROR<int>()) {
    throw std::runtime_error(strerror(errno));
  }
}

void ServerSocket::bind(const SocketAddress& address, int backlog) {
  const int enable = 1;
  if (setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
    throw std::runtime_error(strerror(errno));
  }

  if (::bind(fd_, (sockaddr*)&address.getAddress(), address.getAddressLen()) ==
      ERROR<int>()) {
    throw std::runtime_error(strerror(errno));
  }

  if (fcntl(fd_, F_SETFL, O_NONBLOCK) == ERROR<int>()) {
    throw std::runtime_error(strerror(errno));
  }

  if (listen(fd_, backlog) == ERROR<int>()) {
    throw std::runtime_error(strerror(errno));
  }
  address_ = address;
}

Client ServerSocket::accept() const {
  sockaddr client_addr;
  socklen_t client_addrlen;

  int client_fd = ::accept(fd_, &client_addr, &client_addrlen);
  if (client_fd == ERROR<int>()) {
    throw std::runtime_error(strerror(errno));
  }

  if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == ERROR<int>()) {
    close(client_fd);
    throw std::runtime_error(strerror(errno));
  }

  return Client(client_fd, default_server_,
                SocketAddress(client_addr, client_addrlen), address_);
}

int ServerSocket::getFD() const { return fd_; }
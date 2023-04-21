#ifndef CONNECTION_CLOSED_EXCEPTION_HPP_
#define CONNECTION_CLOSED_EXCEPTION_HPP_

#include <stdexcept>

struct ConnectionClosedException : public std::exception {
  explicit ConnectionClosedException(const int fd) : client_fd(fd) {}

  int client_fd;
};

#endif

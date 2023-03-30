#include "Selector.hpp"

#include <cerrno>
#include <cstring>

#include "constant.hpp"

Selector::Selector() : max_fd_(-1) {
  FD_ZERO(&fds_);
  tm_.tv_sec = 0;
  tm_.tv_usec = 0;
}

Selector::Selector(const Selector& src) { *this = src; }

Selector& Selector::operator=(const Selector& src) {
  if (this != &src) {
    max_fd_ = src.max_fd_;
    fds_ = src.fds_;
    read_fds_ = src.read_fds_;
    write_fds_ = src.write_fds_;
    tm_ = src.tm_;
  }
  return *this;
}

Selector::~Selector() {}

int Selector::select() {
  read_fds_ = fds_;
  write_fds_ = fds_;

  int result = ::select(max_fd_ + 1, &read_fds_, &write_fds_, 0, &tm_);

  if (result == ERROR<int>()) {
    throw SelectFailedException(strerror(errno));
  }

  return result;
}

void Selector::registerFD(int fd) {
  FD_SET(fd, &fds_);
  if (fd > max_fd_) {
    max_fd_ = fd;
  }
}

void Selector::clear(int fd) { FD_CLR(fd, &fds_); }

bool Selector::isReadable(int fd) const { return FD_ISSET(fd, &read_fds_); }
bool Selector::isWritable(int fd) const { return FD_ISSET(fd, &write_fds_); }

Selector::SelectFailedException::SelectFailedException(const char* cause)
    : cause(cause) {}

const char* Selector::SelectFailedException::what() const throw() {
  return cause;
}
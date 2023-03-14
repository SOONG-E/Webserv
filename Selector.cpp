#include "Selector.hpp"

Selector::Selector() : max_fd(-1) {
  FD_ZERO(&read_fds);
  FD_ZERO(&write_fds);
}

Selector::Selector(const Selector &src) { *this = src; }

Selector::~Selector() {}

Selector &Selector::operator=(const Selector &src) {
  if (this != &src) {
    max_fd = src.max_fd;
    memmove(read_fds.fds_bits, src.read_fds.fds_bits,
            __DARWIN_howmany(__DARWIN_FD_SETSIZE, __DARWIN_NFDBITS));
    memmove(write_fds.fds_bits, src.write_fds.fds_bits,
            __DARWIN_howmany(__DARWIN_FD_SETSIZE, __DARWIN_NFDBITS));
  }
  return *this;
}

int Selector::select() {
  int result = ::select(max_fd + 1, &read_fds, &write_fds, 0, 0);
  if (result == -1) throw SelectFailedException(strerror(errno));
  return result;
}

void Selector::registerAll(int fd) {
  registerRead(fd);
  registerWrite(fd);
}

void Selector::registerRead(int fd) {
  FD_SET(fd, &read_fds);
  if (fd > max_fd) max_fd = fd;
}

void Selector::registerWrite(int fd) {
  FD_SET(fd, &write_fds);
  if (fd > max_fd) max_fd = fd;
}

bool Selector::isReadSet(int fd) const {
  return FD_ISSET(fd, &read_fds) ? true : false;
}
bool Selector::isWriteSet(int fd) const {
  return FD_ISSET(fd, &write_fds) ? true : false;
}

void Selector::clear() {
  FD_ZERO(&read_fds);
  FD_ZERO(&write_fds);
}

Selector::SelectFailedException::SelectFailedException(const char *cause)
    : cause(cause) {}

const char *Selector::SelectFailedException::what() const throw() {
  return cause;
}
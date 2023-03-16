#include "Selector.hpp"

Selector::Selector() : _max_fd(-1) { FD_ZERO(&_fds); }

Selector::Selector(const Selector &src) { *this = src; }

Selector::~Selector() {}

Selector &Selector::operator=(const Selector &src) {
  if (this != &src) {
    _max_fd = src._max_fd;
    _fds = src._fds;
    _read_fds = src._read_fds;
    _write_fds = src._write_fds;
  }
  return *this;
}

int Selector::select() {
  _read_fds = _fds;
  _write_fds = _fds;
  int result = ::select(_max_fd + 1, &_read_fds, &_write_fds, 0, 0);
  if (result == -1) throw SelectFailedException(strerror(errno));
  return result;
}

void Selector::registerSocket(int fd) {
  FD_SET(fd, &_fds);
  if (fd > _max_fd) _max_fd = fd;
}
void Selector::clear(int fd) { FD_CLR(fd, &_fds); }

bool Selector::isSetRead(int fd) const { return FD_ISSET(fd, &_read_fds); }
bool Selector::isSetWrite(int fd) const { return FD_ISSET(fd, &_write_fds); }

Selector::SelectFailedException::SelectFailedException(const char *cause)
    : cause(cause) {}

const char *Selector::SelectFailedException::what() const throw() {
  return cause;
}
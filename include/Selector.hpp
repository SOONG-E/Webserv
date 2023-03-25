#ifndef SELECTOR_HPP
#define SELECTOR_HPP

#include <sys/select.h>

#include <cerrno>
#include <cstring>
#include <exception>

class Selector {
 public:
  Selector();
  Selector(const Selector& src);
  ~Selector();

  Selector& operator=(const Selector& src);

  int select();
  void registerFD(int fd);
  void clear(int fd);
  bool isReadable(int fd) const;
  bool isWritable(int fd) const;

 private:
  struct timeval tm_;
  int max_fd_;
  fd_set fds_;
  fd_set read_fds_;
  fd_set write_fds_;

  // exception
 public:
  class SelectFailedException : public std::exception {
   public:
    SelectFailedException(const char* cause);
    const char* what() const throw();

   private:
    const char* cause;
  };
};

#endif
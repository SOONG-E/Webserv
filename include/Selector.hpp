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
  void registerSocket(int fd);
  void clear(int fd);
  bool isSetRead(int fd) const;
  bool isSetWrite(int fd) const;

 private:
  int max_fd_;
  fd_set fds_;
  fd_set read_fds_;
  fd_set write_fds_;

  // exception
 public:
  class SelectFailedException : public std::exception {
   public:
    SelectFailedException(const char* cause);

   private:
    const char* cause;

    const char* what() const throw();
  };
};

#endif
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
  void add(int fd);
  bool isReadSetting(int fd) const;
  bool isWriteSetting(int fd) const;
  void clear();

 private:
  int max_fd;
  fd_set read_fds;
  fd_set write_fds;

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
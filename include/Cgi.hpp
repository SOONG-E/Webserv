#ifndef CGI_HPP
#define CGI_HPP

#include <unistd.h>

#include <map>

#include "HttpRequest.hpp"

class Cgi {
 public:
  Cgi();
  Cgi(const Cgi& src);
  Cgi& operator=(const Cgi& src);
  ~Cgi();

  void executeCgiScript(const HttpRequest& request_obj);

 private:
  char** makeEnvp(const HttpRequest& request_obj) const;

  enum e_pipe_fd { READ_FD = 0, WRITE_FD = 1 };

  std::string buf_;
  int pipe_fds_[2];
};

#endif
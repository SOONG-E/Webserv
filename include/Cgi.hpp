#ifndef CGI_HPP_
#define CGI_HPP_

#include <unistd.h>

#include <csignal>
#include <map>
#include <string>

#include "HttpRequest.hpp"

enum e_pipe_fd { READ = 0, WRITE = 1 };

class Cgi {
 public:
  Cgi();
  Cgi(const Cgi& src);
  Cgi& operator=(const Cgi& src);
  ~Cgi();

  void runCgiScript(const HttpRequest& request_obj);
  void readPipe();
  void writePipe();
  bool isCompleted() const;
  bool isWriteCompleted() const;
  std::string getCgiResponse() const;
  int* getPipe();
  const int* getPipe() const;
  void clear();

 private:
  char** makeEnvp(const HttpRequest& request_obj) const;

  bool is_completed_;
  pid_t script_pid_;
  std::string write_buf_;
  std::string read_buf_;
  int pipe_fds_[2];
};

#endif

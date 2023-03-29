#ifndef CGI_HPP_
#define CGI_HPP_

#include <unistd.h>

#include <csignal>
#include <map>
#include <string>

#include "Client.hpp"
#include "HttpRequest.hpp"
#include "SocketAddress.hpp"
#include "utility.hpp"

enum e_pipe_fd { READ = 0, WRITE = 1 };

class Cgi {
 public:
  Cgi();
  Cgi(const Cgi& src);
  Cgi& operator=(const Cgi& src);
  ~Cgi();

  void runCgiScript(const Client& client);
  void readPipe();
  void writePipe();
  bool isCompleted() const;
  bool isWriteCompleted() const;
  const std::string& getCgiResponse() const;
  int* getPipe();
  const int* getPipe() const;
  void clear();

 private:
  char** generateEnvp(const HttpRequest& request_obj,
                      const SocketAddress& cli_addr,
                      const SocketAddress& serv_addr) const;
  std::string getAbsolutePath(const std::string& uri) const;

  bool is_completed_;
  pid_t pid_;
  std::string write_buf_;
  std::string read_buf_;
  int pipe_fds_[2];
};

#endif

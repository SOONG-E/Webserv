#ifndef CGI_HPP_
#define CGI_HPP_

#include <fcntl.h>
#include <unistd.h>

#include <csignal>
#include <ctime>
#include <map>
#include <string>

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Selector.hpp"
#include "SocketAddress.hpp"
#include "utility.hpp"

enum PipeFD { READ = 0, WRITE = 1 };

class Cgi {
 public:
  Cgi();
  Cgi(const Cgi& src);
  Cgi& operator=(const Cgi& src);
  ~Cgi();

  void execute(const HttpRequest& request, const HttpResponse& response,
               const SocketAddress& cli_addr, const SocketAddress& serv_addr);
  void write(Selector& selector);
  void read(Selector& selector, HttpResponse& response);

  int getWriteFD() const;
  int getReadFD() const;
  std::time_t getTimeout() const;

  bool isCompleted() const;
  bool hasBody() const;

  void clear();
  void cleanUp(Selector& selector) const;

 private:
  char** generateEnvp(const HttpRequest& request, const HttpResponse& response,
                      const SocketAddress& cli_addr,
                      const SocketAddress& serv_addr) const;
  void deleteEnvp(char** envp) const;
  std::string getAbsolutePath(const std::string& uri) const;

  bool is_completed_;
  int pipe_fds_[2];
  std::string body_;
  pid_t pid_;
  std::time_t timeout_;
};

#endif

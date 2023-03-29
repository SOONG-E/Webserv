#include "Cgi.hpp"

#include <cstdio>

#include "ResponseStatus.hpp"
#include "constant.hpp"
#include "exception.hpp"

Cgi::Cgi() : is_completed_(false), pid_(-1) {
  pipe_fds_[READ] = -1;
  pipe_fds_[WRITE] = -1;
}

Cgi::Cgi(const Cgi& src) { *this = src; }

Cgi& Cgi::operator=(const Cgi& src) {
  if (this != &src) {
    write_buf_ = src.write_buf_;
    read_buf_ = src.read_buf_;
    pipe_fds_[READ] = src.pipe_fds_[READ];
    pipe_fds_[WRITE] = src.pipe_fds_[WRITE];
    pid_ = src.pid_;
    is_completed_ = src.is_completed_;
  }
  return *this;
}

Cgi::~Cgi() {
  close(pipe_fds_[READ]);
  close(pipe_fds_[WRITE]);
}

void Cgi::runCgiScript(const HttpRequest& request_obj,
                       const SocketAddress& cli_addr,
                       const SocketAddress& serv_addr,
                       const std::string& cgi_path) {
  char* argv[2];
  int pipe_fds1[2];
  int pipe_fds2[2];

  char** envp = generateEnvp(request_obj, cli_addr, serv_addr);

  argv[0] = const_cast<char*>(cgi_path.c_str());
  argv[1] = NULL;
  if (pipe(pipe_fds1) == -1) {
    throw ResponseException(C500);
  }
  if (pipe(pipe_fds2) == -1) {
    close(pipe_fds1[READ]);
    close(pipe_fds1[WRITE]);
    throw ResponseException(C500);
  }
  pid_ = fork();
  if (pid_ == -1) {
    close(pipe_fds1[READ]);
    close(pipe_fds1[WRITE]);
    close(pipe_fds2[READ]);
    close(pipe_fds2[WRITE]);
    throw ResponseException(C500);
  } else if (pid_ == 0) {
    dup2(pipe_fds1[READ], STDIN_FILENO);
    dup2(pipe_fds2[WRITE], STDOUT_FILENO);
    close(pipe_fds1[WRITE]);
    close(pipe_fds2[READ]);
    if (execve(argv[0], argv, envp) == -1) {
      exit(EXIT_FAILURE);
    }
  }

  for (int i = 0; envp[i]; ++i) {
    delete[] envp[i];
  }
  delete[] envp;

  pipe_fds_[READ] = pipe_fds2[READ];
  pipe_fds_[WRITE] = pipe_fds1[WRITE];
  close(pipe_fds1[READ]);
  close(pipe_fds2[WRITE]);

  if (request_obj.getMethod() == "GET") {
    close(pipe_fds_[WRITE]);
  } else {
    write_buf_ = request_obj.getBody();
  }
}

bool Cgi::isCompleted() const { return is_completed_; }

bool Cgi::isWriteCompleted() const { return write_buf_.empty(); }

const std::string& Cgi::getCgiResponse() const { return read_buf_; }

char** Cgi::generateEnvp(const HttpRequest& request_obj,
                         const SocketAddress& cli_addr,
                         const SocketAddress& serv_addr) const {
  std::map<std::string, std::string> env_map;

  env_map["AUTH_TYPE"] = "";
  env_map["CONTENT_LENGTH"] = toString(request_obj.getContentLength());
  env_map["CONTENT_TYPE"] = request_obj.getHeader("CONTENT-TYPE");
  env_map["GATEWAY_INTERFACE"] = "CGI/1.1";
  env_map["PATH_INFO"] = request_obj.getUri();
  env_map["PATH_TRANSLATED"] = getAbsolutePath(request_obj.getUri());
  env_map["QUERY_STRING"] = request_obj.getQueryString();
  env_map["REMOTE_HOST"] = "";
  env_map["REMOTE_ADDR"] = cli_addr.getIP();
  env_map["REMOTE_USER"] = "";
  env_map["REMOTE_IDENT"] = "";
  env_map["REQUEST_METHOD"] = request_obj.getMethod();
  env_map["REQUEST_URI"] = request_obj.getUri();
  env_map["SCRIPT_NAME"] = request_obj.getUri();
  env_map["SERVER_NAME"] = serv_addr.getIP();
  env_map["SERVER_PROTOCOL"] = "HTTP/1.1";
  env_map["SERVER_PORT"] = serv_addr.getPort();
  env_map["SERVER_SOFTWARE"] = "webserv/1.1";

  char** envp = new char*[env_map.size() + 1];

  int i = 0;
  for (std::map<std::string, std::string>::iterator it = env_map.begin();
       it != env_map.end(); ++it, ++i) {
    envp[i] = strdup((it->first + "=" + it->second).c_str());
    if (envp[i] == NULL) {
      for (int j = 0; j < i; ++j) {
        delete[] envp[j];
      }
      delete[] envp;
      throw ResponseException(C500);
    }
  }
  envp[i] = NULL;
  return envp;
}

void Cgi::readPipe() {
  char buf[BUF_SIZE + 1];

  size_t read_bytes = read(pipe_fds_[READ], buf, BUF_SIZE);
  if (static_cast<ssize_t>(read_bytes) == -1) {
    close(pipe_fds_[WRITE]);
    close(pipe_fds_[READ]);
    kill(pid_, SIGTERM);
    throw ResponseException(C500);
  }
  if (read_bytes == 0) {
    is_completed_ = true;
    return;
  }
  read_buf_ += std::string(buf, read_bytes);
}

void Cgi::writePipe() {
  size_t write_bytes =
      write(pipe_fds_[WRITE], write_buf_.c_str(), write_buf_.size());
  if (static_cast<ssize_t>(write_bytes) == -1) {
    close(pipe_fds_[WRITE]);
    close(pipe_fds_[READ]);
    kill(pid_, SIGTERM);
    throw ResponseException(C500);
  }
  if (write_bytes < write_buf_.size()) {
    write_buf_.erase(0, write_bytes);
  }
}

int* Cgi::getPipe() { return pipe_fds_; }

const int* Cgi::getPipe() const { return pipe_fds_; }

void Cgi::clear() {
  close(pipe_fds_[READ]);
  close(pipe_fds_[WRITE]);
  write_buf_.clear();
  read_buf_.clear();
  is_completed_ = false;
  pid_ = -1;
  pipe_fds_[READ] = -1;
  pipe_fds_[WRITE] = -1;
}

std::string Cgi::getAbsolutePath(const std::string& uri) const {
  char buf[FILENAME_MAX];
  if (!getcwd(buf, FILENAME_MAX)) {
    throw ResponseException(C500);
  }
  std::string pwd(buf);
  return pwd + uri;
}

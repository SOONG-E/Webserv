#include "Cgi.hpp"

Cgi::Cgi() {
  pipe_fds_[READ_FD] = -1;
  pipe_fds_[WRITE_FD] = -1;
}

Cgi::Cgi(const Cgi& src) { *this = src; }

Cgi& Cgi::operator=(const Cgi& src) {
  if (this != &src) {
    buf_ = src.buf_;
    pipe_fds_[READ_FD] = src.pipe_fds_[READ_FD];
    pipe_fds_[WRITE_FD] = src.pipe_fds_[WRITE_FD];
  }
  return *this;
}

Cgi::~Cgi() {
  close(pipe_fds_[READ_FD]);
  close(pipe_fds_[WRITE_FD]);
}

void Cgi::executeCgiScript(const HttpRequest& request_obj) {
  char* argv[2];
  char** envp;
  int pipe_fds1[2];
  int pipe_fds2[2];

  envp = makeEnvp(request_obj);
  // argv[0] = cgi path
  argv[1] = NULL;
  if (pipe(pipe_fds1) == -1) {
    // throw error; 500
  }
  if (pipe(pipe_fds2) == -1) {
    // close all fd
    //  throw error; 500
  }
  pid_t pid = fork();
  if (pid == -1) {
    // close all fd
    //  throw error; 500
  } else if (pid == 0) {
    dup2(pipe_fds1[READ_FD], STDIN_FILENO);
    dup2(pipe_fds2[WRITE_FD], STDOUT_FILENO);
    close(pipe_fds1[WRITE_FD]);
    close(pipe_fds2[READ_FD]);
    if (execve(argv[0], argv, envp) == -1) {
      exit(EXIT_FAILURE);
    }
  }
  pipe_fds_[READ_FD] = pipe_fds2[READ_FD];
  pipe_fds_[WRITE_FD] = pipe_fds1[WRITE_FD];
  close(pipe_fds1[READ_FD]);
  close(pipe_fds2[WRITE_FD]);
}

char** Cgi::makeEnvp(const HttpRequest& request_obj) const {
  std::map<std::string, std::string> env_map;

  env_map["AUTH_TYPE"] = "";
  env_map["CONTENT_LENGTH"] = request_obj.getContentLength();
  env_map["CONTENT_TYPE"] = request_obj.getHeader("Content-Length");
  env_map["GATEWAY_INTERFACE"] = "CGI/1.1";
  env_map["PATH_INFO"] = request_obj.getUri();
  // env_map["PATH_TRANSLATED"] = pull_path
  // env_map["QUERY_STRING"] = request_obj.getQueryString();
  env_map["REMOTE_HOST"] = "";
  // env_map["REMOTE_ADDR"] = client_ip;
  env_map["REMOTE_USER"] = "";
  env_map["REMOTE_IDENT"] = "";
  env_map["REQUEST_METHOD"] = request_obj.getMethod();
  env_map["REQUEST_URI"] = request_obj.getUri();
  env_map["SCRIPT_NAME"] = request_obj.getUri();
  // env_map["SERVER_NAME"] = server ip or server_name
  env_map["SERVER_PROTOCOL"] = "HTTP/1.1";
  // env_map["SERVER_PORT"] = server_port
  env_map["SERVER_SOFTWARE"] = "webserv/1.1";

  char** envp = new (std::nothrow) char*[env_map.size() + 1];
  if (envp == NULL) {
    // throw 500;
  }

  int i = 0;
  for (std::map<std::string, std::string>::iterator it = env_map.begin();
       it != env_map.end(); ++it, ++i) {
    envp[i] = strdup((it->first + "=" + it->second).c_str());
    if (envp[i] == NULL) {
      for (int j = 0; j < i; ++j) {
        delete[] envp[j];
      }
      delete[] envp;
    }
    // throw 500
  }
  envp[i] = NULL;
  return envp;
}
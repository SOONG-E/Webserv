#include "CgiHandler.hpp"

/*======================//
 execute Cgi
========================*/

void CgiHandler::execute(Client* client) {
  int pipe_fds[2][2];
  Process process;

  std::string cgi_path = client->getLocation().getCgiParam("CGI_PATH");
  std::string uri = getAbsolutePath(client->getRequest().getUri());

  if (pipe(pipe_fds[0]) == ERROR<int>()) {
    throw ResponseException(C500);
  }
  if (pipe(pipe_fds[1]) == ERROR<int>()) {
    close(pipe_fds[0][READ]);
    close(pipe_fds[0][WRITE]);

    throw ResponseException(C500);
  }

  char* argv[3] = {const_cast<char*>(cgi_path.c_str()),
                   const_cast<char*>(uri.c_str()), NULL};
  char** envp = generateEnvp(client);

  int pid = fork();

  if (pid == ERROR<pid_t>()) {
    deleteEnvp(envp);
    close(pipe_fds[0][READ]);
    close(pipe_fds[0][WRITE]);
    close(pipe_fds[1][READ]);
    close(pipe_fds[1][WRITE]);

    throw ResponseException(C500);
  } else if (pid == 0) {
    close(pipe_fds[0][WRITE]);
    close(pipe_fds[1][READ]);
    dup2(pipe_fds[0][READ], STDIN_FILENO);
    dup2(pipe_fds[1][WRITE], STDOUT_FILENO);

    execve(argv[0], argv, envp);
    exit(EXIT_FAILURE);
  }

  deleteEnvp(envp);
  close(pipe_fds[0][READ]);
  close(pipe_fds[1][WRITE]);

  process.pid = pid;
  process.input_fd = pipe_fds[1][READ];
  process.output_fd = pipe_fds[0][WRITE];

  if (fcntl(process.input_fd, F_SETFL, O_NONBLOCK) == ERROR<int>() ||
      fcntl(process.output_fd, F_SETFL, O_NONBLOCK) == ERROR<int>()) {
    cleanUp(client);
    throw ResponseException(C500);
  }

  if (client->getRequest().getMethod() != METHODS[POST]) {
    close(process.output_fd);
  }
  process.message_to_send = client->getRequest().getBody();
  client->setProcess(process);

  setPhase(client, P_WRITE);
  client->setAllTimeout();
  setTimer(client);
}

/*======================================//
 process depending on event_type(phase)
========================================*/

void CgiHandler::handle(Client* client, int event_type) {
  switch (event_type) {
    case EVFILT_READ:
      readFromCgi(client);
      break;
    case EVFILT_WRITE:
      sendToCgi(client);
      break;
    case EVFILT_PROC:
      setPhase(client, P_READ);
      break;
    case EVFILT_TIMER:
      throw ResponseException(C500);
  }
}
/*=========================//
 WRITE Phase
===========================*/

void CgiHandler::sendToCgi(Client* client) {
  if (client->getProcess().phase != P_WRITE) {
    return;
  }
  Process& process = client->getProcess();
  std::size_t write_bytes;

  write_bytes = ::write(process.output_fd, process.message_to_send.c_str(),
                        process.message_to_send.size());
  if (write_bytes == ERROR<std::size_t>()) {
    throw ResponseException(C500);
  }

  process.message_to_send.erase(0, write_bytes);
  if (process.message_to_send.empty() == true) {
    close(process.output_fd);
    setPhase(client, P_WAIT);
  }
}

/*=========================//
 READ Phase
===========================*/

void CgiHandler::readFromCgi(Client* client) {
  if (client->getProcess().phase != P_READ) {
    return;
  }
  Process& process = client->getProcess();
  char buffer[BUFFER_SIZE];

  std::size_t read_bytes = ::read(process.input_fd, buffer, BUFFER_SIZE);

  if (read_bytes == ERROR<std::size_t>()) {
    throw ResponseException(C500);
  }
  if (read_bytes == 0) {
    setPhase(client, P_DONE);
  }
  process.message_received += std::string(buffer, read_bytes);
}

/*=========================//
 get data after cgi is done
===========================*/

struct Response CgiHandler::getResponse(Client* client) {
  Response response;
  Process& process = client->getProcess();
  process.phase = P_UNSTARTED;

  const std::string& message = process.message_received;
  std::size_t boundary = message.find(DOUBLE_LF);

  if (boundary == NPOS) {
    response.headers = generateHeader(message);
    return response;
  }
  response.headers = generateHeader(message.substr(0, boundary));
  response.body = message.substr(boundary + DOUBLE_LF.size());

  return response;
}

std::map<std::string, std::string> CgiHandler::generateHeader(
    const std::string& headers) {
  std::map<std::string, std::string> splited_header;
  std::vector<std::string> header = split(headers, LF);
  std::vector<std::string> pair;

  for (std::vector<std::string>::iterator it = header.begin();
       it != header.end(); ++it) {
    pair = split(*it, ":");
    splited_header[trim(pair[0])] = trim(pair[1]);
  }
  return splited_header;
}

/*=========================//
 set Timer
===========================*/

void CgiHandler::setTimer(Client* client) {
  if (KEEPALIVE_TIMEOUT < CGI_TIMEOUT || SESSION_TIMEOUT < CGI_TIMEOUT) {
    return;
  }
  client->getServerManager()->createEvent(client->getFd(), EVFILT_TIMER,
                                          EV_ADD | EV_ONESHOT, NOTE_SECONDS,
                                          CGI_TIMEOUT, client);
}

/*=========================//
 utils
===========================*/

/* generate Envp for CGI */
char** CgiHandler::generateEnvp(const Client* client) {
  const HttpRequest& request = client->getRequest();
  const TcpServer* server = client->getTcpServer();
  std::map<std::string, std::string> env_map;

  const std::string& method = request.getMethod();
  std::size_t content_length = request.getContentLength();
  if (method == "POST" && content_length > 0) {
    env_map["CONTENT_LENGTH"] = toString(content_length);
  }
  env_map["AUTH_TYPE"] = "";
  env_map["CONTENT_TYPE"] = request.getHeader("CONTENT-TYPE");
  env_map["GATEWAY_INTERFACE"] = "CGI/1.1";
  env_map["PATH_INFO"] = request.getUri();
  env_map["PATH_TRANSLATED"] = getAbsolutePath(request.getUri());
  env_map["QUERY_STRING"] = request.getQueryString();
  env_map["REMOTE_HOST"] = "";
  env_map["REMOTE_ADDR"] = client->getAddr().getIP();
  env_map["REMOTE_USER"] = "";
  env_map["REMOTE_IDENT"] = "";
  env_map["REQUEST_METHOD"] = method;
  env_map["REQUEST_URI"] = request.getUri();
  env_map["SCRIPT_NAME"] = request.getUri();
  env_map["SERVER_NAME"] = server->getIp();
  env_map["SERVER_PROTOCOL"] = "HTTP/1.1";
  env_map["SERVER_PORT"] = server->getPort();
  env_map["SERVER_SOFTWARE"] = "webserv/1.1";

  env_map["HTTP_X_SERVER_KEY"] =
      toString(client->getHttpServer()->getServerKey());
  const Session* session = client->getSession();
  if (session) {
    env_map["HTTP_X_SESSION_ID"] = session->getID();
  }

  char** envp = new char*[env_map.size() + 1];

  int i = 0;
  for (std::map<std::string, std::string>::const_iterator it = env_map.begin();
       it != env_map.end(); ++it, ++i) {
    envp[i] = strdup((it->first + "=" + it->second).c_str());
    if (envp[i] == NULL) {
      deleteEnvp(envp);
      throw ResponseException(C500);
    }
  }
  envp[i] = NULL;
  return envp;
}

/* delete Envp */
void CgiHandler::deleteEnvp(char** envp) {
  for (int i = 0; envp[i]; ++i) {
    delete[] envp[i];
  }
  delete[] envp;
}

/* generate absolute path using uri */
std::string CgiHandler::getAbsolutePath(const std::string& uri) {
  char buf[FILENAME_MAX];
  if (!getcwd(buf, FILENAME_MAX)) {
    throw ResponseException(C500);
  }
  return buf + uri;
}

/* set kevent depending on phase */
void CgiHandler::setPhase(Client* client, int phase) {
  ServerManager* manager = client->getServerManager();
  Process& process = client->getProcess();

  switch (phase) {
    case P_WRITE:
      manager->createEvent(client->getFd(), EVFILT_READ, EV_DISABLE, 0, 0,
                           client);
      manager->createEvent(process.output_fd, EVFILT_WRITE, EV_ADD | EV_ENABLE,
                           0, 0, client);
      process.phase = P_WRITE;
      break;

    case P_WAIT:
      manager->createEvent(process.pid, EVFILT_PROC, EV_ADD | EV_ENABLE,
                           NOTE_EXIT, 0, client);
      process.phase = P_WAIT;

      break;

    case P_READ:
      manager->createEvent(process.pid, 0, EV_DELETE, 0, 0, client);
      manager->createEvent(process.input_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0,
                           0, client);
      process.phase = P_READ;
      break;

    case P_DONE:
      close(process.input_fd);
      if (CGI_TIMEOUT < KEEPALIVE_TIMEOUT && CGI_TIMEOUT < SESSION_TIMEOUT) {
        manager->createEvent(client->getFd(), EVFILT_TIMER, EV_DELETE, 0, 0,
                             client);
      }
      manager->createEvent(client->getFd(), EVFILT_READ, EV_ENABLE, 0, 0,
                           client);
      process.phase = P_DONE;
      break;

    case P_RESET:
      manager->createEvent(client->getFd(), EVFILT_READ, EV_ENABLE, 0, 0,
                           client);
      manager->createEvent(process.output_fd, EVFILT_WRITE, EV_DELETE, 0, 0,
                           client);
      manager->createEvent(process.pid, EVFILT_PROC, EV_DELETE, 0, 0, client);
      manager->createEvent(process.input_fd, EVFILT_READ, EV_DELETE, 0, 0,
                           client);
      manager->createEvent(client->getFd(), EVFILT_TIMER, EV_DELETE, 0, 0,
                           client);
      process.phase = P_UNSTARTED;
      cleanUp(client);
  }
}

void CgiHandler::cleanUp(Client* client) {
  Process& process = client->getProcess();
  kill(process.pid, SIGKILL);
  if (process.input_fd != -1) {
    close(process.input_fd);
  }
  if (process.output_fd != -1) {
    close(process.output_fd);
  }
}
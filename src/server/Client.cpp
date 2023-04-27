#include "Client.hpp"

#include <cerrno>
#include <cstring>
#include <iostream>

Client::Client(const int fd, const TcpServer* tcp_server,
               const SocketAddress& address, ServerManager* manager)
    : manager_(manager),
      fd_(fd),
      session_(NULL),
      tcp_server_(tcp_server),
      address_(address),
      http_server_(NULL),
      status_(C200),
      is_response_ready_(false) {}

Client::Client(const Client& origin)
    : manager_(origin.manager_),
      fd_(origin.fd_),
      tcp_server_(origin.tcp_server_),
      address_(origin.address_),
      http_server_(origin.http_server_),
      location_(origin.location_),
      request_(origin.request_),
      cgi_process_(origin.cgi_process_),
      fullUri_(origin.fullUri_),
      response_(origin.response_),
      status_(origin.status_),
      is_response_ready_(origin.is_response_ready_) {}

Client Client::operator=(const Client& origin) { return Client(origin); }

Client::~Client() {}

/*======================//
 Getter
========================*/

ServerManager* Client::getServerManager(void) { return manager_; }
int Client::getFd() const { return fd_; }
const TcpServer* Client::getTcpServer(void) const { return tcp_server_; }
HttpServer* Client::getHttpServer(void) const { return http_server_; }
const SocketAddress Client::getAddr(void) const { return address_; }
Location& Client::getLocation(void) { return location_; }
const Location& Client::getLocation(void) const { return location_; }
HttpRequest& Client::getRequest(void) { return request_; }
const HttpRequest& Client::getRequest(void) const { return request_; }
Process& Client::getProcess(void) { return cgi_process_; }
std::string& Client::getResponse(void) { return response_; }
const std::string& Client::getResponse(void) const { return response_; }
int& Client::getStatus(void) { return status_; }
const int& Client::getStatus(void) const { return status_; }
std::string& Client::getFullUri(void) { return fullUri_; }
const std::string& Client::getFullUri(void) const { return fullUri_; }

/*======================//
 Setter
========================*/

void Client::setStatus(int status) { status_ = status; }
void Client::setSession(Session* session) { session_ = session; }
void Client::setProcess(Process& cgi_process) { cgi_process_ = cgi_process; }

/*======================//
 process
========================*/

/* recognize a type of event */
void Client::processEvent(const int event_type) {
  if (isCgiStarted() == true) {
    passToCgi(event_type);
    return;
  }
  switch (event_type) {
    case EVFILT_READ:
      processRequest();
      break;

    case EVFILT_WRITE:
      writeData();
      break;

    default:
      throw std::runtime_error(strerror(errno));  // 수정!
  }
}

/* parse request and pass it to handler */
void Client::processRequest(void) {
  try {
    std::string data = readData();
    request_.tailRequest(data);
    request_.parse();

    if (request_.isCompleted() == true) {
      lookUpHttpServer();
      lookUpLocation();
      setFullUri();
      passRequestToHandler();
      setSession();
      validAuth();
    }
  } catch (const ResponseException& e) {
    passErrorToHandler(e.status);
  } catch (const ConnectionClosedException& e) {
    throw e;
  } catch (std::runtime_error& e) {
    throw e;
  } catch (const std::exception& e) {
    passErrorToHandler(C500);
  }
}

/* read data from a connection socket */
std::string Client::readData(void) {
  char buffer[BUFFER_SIZE];
  std::size_t read_bytes = recv(fd_, &buffer, BUFFER_SIZE, 0);

  if (read_bytes == static_cast<std::size_t>(-1)) {
    throw ConnectionClosedException(fd_);
  }
  if (read_bytes == 0) {
    throw std::runtime_error("no data in buffer");
  }
  return std::string(buffer, read_bytes);
}

/* lookup associated virtual server
if there isn't a matched server then default server is setted */
void Client::lookUpHttpServer(void) {
  const TcpServer::VirtualServerType virtual_servers =
      tcp_server_->getVirtualServers();
  std::map<std::string, const HttpServer*>::const_iterator server =
      virtual_servers.find(request_.getHost());
  if (server == virtual_servers.end()) {
    http_server_ = tcp_server_->getDefaultServer();
    return;
  }
  http_server_ = const_cast<HttpServer*>(server->second);
}

/* lookup associated location block */
void Client::lookUpLocation(void) {
  location_ = http_server_->findLocation(request_.getUri());
}

/* turn the uri into full uri */
void Client::setFullUri(void) {
  std::string root = location_.getRoot();
  if (*root.rbegin() != '/') {
    root += "/";
  }
  std::string uri = request_.getUri();
  fullUri_ = uri.replace(0, location_.getUri().size(), root);
}

void Client::setSession(void) {
  if (request_.hasCookie() == false) {
    return;
  }
  const std::string session_id = request_.getCookie(SESSION_ID_FIELD);
  session_ = http_server_->getSession(session_id);
}

void Client::validAuth(void) {
  if (location_.getAuth() == false) {
    return;
  }
  if (session_ == NULL) {
    throw ResponseException(C403);
  }
}

/* set status code and pass it to the handler */
void Client::passErrorToHandler(int status) {
  status_ = status;
  passRequestToHandler();
}

/* pass the request to eligible handler */
void Client::passRequestToHandler(void) {
  struct Response response_from_upsteam;
  try {
    if (location_.isCgi() == true && isErrorCode() == false) {
      if (isCgiStarted() == false) {
        CgiHandler::execute(this);
        return;
      }
      response_from_upsteam = CgiHandler::getResponse(this);
    } else if (SessionHandler::recognizeRequest(request_) == true) {
      response_from_upsteam = SessionHandler::handle(this);
    } else if (location_.getAutoindex() == true && isErrorCode() == false) {
      response_from_upsteam = AutoIndexHandler::handle(this);
    } else {
      response_from_upsteam = StaticContentHandler::handle(this);
    }
  } catch (const ResponseException& e) {
    status_ = e.status;
    response_from_upsteam = StaticContentHandler::handle(this);
  }
  response_ = ResponseGenerator::generateResponse(*this, response_from_upsteam);
  setToSend(true);
}

void Client::passToCgi(const int event_type) {
  try {
    CgiHandler::handle(this, event_type);
    if (isCgiDone() == true) {
      passRequestToHandler();
    }
  } catch (const ResponseException& e) {
    CgiHandler::setPhase(this, P_RESET);
    passErrorToHandler(e.status);
  }
}

/* send the response to client */
void Client::writeData(void) {
  std::size_t write_bytes;
  if (is_response_ready_ == false) {
    return;
  }
  write_bytes = ::send(fd_, response_.c_str(), response_.size(), 0);
  if (write_bytes == ERROR<std::size_t>()) {
    throw ConnectionClosedException(fd_);
  }

  response_.erase(0, write_bytes);
  if (response_.empty() == true) {
    setToSend(false);
    clear();
  }
}

/*======================//
 utils
========================*/

/* turn on/off event, is_response_ready */
void Client::setToSend(bool set) {
  is_response_ready_ = set;
  if (set == true) {
    manager_->createEvent(fd_, EVFILT_READ, EV_DISABLE, 0, 0, this);
    manager_->createEvent(fd_, EVFILT_WRITE, EV_ENABLE, 0, 0, this);
    return;
  }
  manager_->createEvent(fd_, EVFILT_READ, EV_ENABLE, 0, 0, this);
  manager_->createEvent(fd_, EVFILT_WRITE, EV_DISABLE, 0, 0, this);
}

bool Client::isErrorCode(void) {
  if (status_ < C400) {
    return false;
  }
  return true;
}

bool Client::isCgiStarted(void) { return (cgi_process_.phase != P_UNSTARTED); }

bool Client::isCgiDone(void) { return (cgi_process_.phase == P_DONE); }

void Client::clear() {
  request_.clear();
  fullUri_.clear();
  status_ = C200;
  is_response_ready_ = false;
  cgi_process_.phase = P_UNSTARTED;
}

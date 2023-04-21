#include "Client.hpp"

#include <cerrno>
#include <cstring>
#include <iostream>

Client::Client(const int fd, const TcpServer* tcp_server,
               const SocketAddress& address, ServerManager* manager)
    : manager_(manager),
      fd_(fd),
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
      response_(origin.response_),
      status_(origin.status_),
      is_response_ready_(origin.is_response_ready_) {}

Client::~Client() {}

/*======================//
 Getter
========================*/

int Client::getFd() const { return fd_; }
HttpServer* Client::getHttpServer(void) const { return http_server_; }
Location& Client::getLocation(void) { return location_; }
const Location& Client::getLocation(void) const { return location_; }
HttpRequest& Client::getRequest(void) { return request_; }
const HttpRequest& Client::getRequest(void) const { return request_; }
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

/*======================//
 process
========================*/

/* recognize type of event */
void Client::processEvent(const int event_type) {
  switch (event_type) {
    case EVFILT_READ:
      processRequest();
      break;

    case EVFILT_WRITE:
      writeData();
      break;

    case EVFILT_PROC:
      //
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
    }
  } catch (const ResponseException& e) {
    passErrorToHandler(e.status);
  } catch (const ConnectionClosedException& e) {
    throw e;
  } catch (const std::exception& e) {
    passErrorToHandler(C500);
  }
}

/* read data from socket */
std::string Client::readData(void) {
  char buffer[BUFFER_SIZE];
  std::size_t read_bytes = recv(fd_, &buffer, BUFFER_SIZE, 0);

  if (read_bytes == static_cast<std::size_t>(-1)) {
    throw ConnectionClosedException(fd_);
  }
  if (read_bytes == 0) {
    throw ConnectionClosedException(fd_);
  }
  return std::string(buffer);
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

/* turn uri into full uri */
void Client::setFullUri(void) {
  std::string root = location_.getRoot();
  if (*root.rbegin() != '/') {
    root += "/";
  }
  std::string uri = request_.getUri();
  fullUri_ = uri.replace(0, location_.getUri().size(), root);
}

/* set status code and pass it to handler */
void Client::passErrorToHandler(int status) {
  status_ = status;
  passRequestToHandler();
}

/* pass the request to eligible handler */
void Client::passRequestToHandler(void) {
  struct Response response_from_upsteam;
  try {
    if (location_.isCgi() == true && isErrorCode() == false) {
      // cgi handler
    }
    if (location_.getAutoindex() == true && isErrorCode() == false) {
      // autoindex handle
    }
    response_from_upsteam = StaticContentHandler::handle(this);
  } catch (const ResponseException& e) {
    status_ = e.status;
    response_from_upsteam = StaticContentHandler::handle(this);
  }
  response_ = ResponseGenerator::generateResponse(*this, response_from_upsteam);
  setToSend(true);
}

/* turn on/off event, is_response_ready */
void Client::setToSend(bool set) {
  is_response_ready_ = set;
  if (set == true) {
    manager_->createEvent(fd_, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, this);
    manager_->createEvent(fd_, EVFILT_READ, EV_ADD | EV_DISABLE, 0, 0, this);
    return;
  }
  manager_->createEvent(fd_, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, this);
  manager_->createEvent(fd_, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, this);
}

/* send response to client */
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
    clearClient();
  }
}

/*======================//
 utils
========================*/

bool Client::isErrorCode(void) {
  if (status_ <= C204) {
    return false;
  }
  return true;
}

void Client::clearClient() { request_ = HttpRequest(); }

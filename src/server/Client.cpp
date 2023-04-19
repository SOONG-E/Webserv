#include "Client.hpp"

#include <cerrno>
#include <cstring>
#include <iostream>

Client::Client(const int fd, const TcpServer* tcp_server,
               const SocketAddress& address)
    : fd_(fd),
      tcp_server_(tcp_server),
      address_(address),
      http_server_(NULL),
      is_response_ready_(false) {}

Client::Client(const Client& origin)
    : fd_(origin.fd_),
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
HttpRequest Client::getRequest(void) const { return request_; }
std::string Client::getResponse(void) const { return response_; }
std::string Client::getStatus(void) const { return status_; }
int Client::getStatusInt(void) const { return ::stoi(status_); }
std::string Client::getFullUri(void) const { return fullUri_; }

/*======================//
 Setter
========================*/

void Client::setStatus(int status) { status_ = toString(status); }
void Client::setStatus(std::string& status) { status_ = status; }

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
  std::string data = readData();
  request_.tailRequest(data);
  try {
    request_.parse();
  } catch (std::exception& e) {
    // error handling
  }
  if (request_.isCompleted() == true) {
    lookUpHttpServer();
    lookUpLocation();
    setFullUri();
    passRequestToHandler();
  }
  //
}

/* read data from socket */
std::string Client::readData(void) {
  char buffer[BUFFER_SIZE];
  std::size_t read_bytes = recv(fd_, &buffer, BUFFER_SIZE, 0);

  if (read_bytes == static_cast<std::size_t>(-1)) {
    throw;
  }
  if (read_bytes == 0) {
    throw;
  }
  return std::string(buffer);
}

/* lookup associated virtual server
if there isn't a matched server then default server is setted */
void Client::lookUpHttpServer(void) {
  const std::map<std::string, const HttpServer*> virtual_servers =
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

void Client::setFullUri(void) {
  std::string root = location_.getRoot();
  if (*root.rbegin() != '/') {
    root += "/";
  }
  std::string uri = request_.getUri();
  fullUri_ = uri.replace(0, location_.getUri().size(), root);
}

/* pass the request to eligible handler */
void Client::passRequestToHandler(void) {
  struct Response response_from_upsteam;
  try {
    if (location_.isCgi() == true) {
      // cgi handler
    }
    if (location_.getAutoindex() == true) {
      // autoindex handle
    }
    // static handler
    // response_ = ResponseGenerator(response_from_upsteam);
    // setToSend(true);
  } catch (std::exception& e) {
    // static handler
  }
}

/* turn on/off event, is_response_ready */
void Client::setToSend(bool set) {
  is_response_ready_ = set;
  if (set == true) {
    // 이벤트 처리로 writable기다릴 수 있게 하기
    return;
  }
  // writable 이벤트 끄기
}

/* send response to client */
void Client::writeData(void) {
  std::size_t write_bytes;
  if (is_response_ready_ == true) {
    write_bytes = ::send(fd_, response_.c_str(), response_.size(), 0);
    if (write_bytes == -1) {
      // error handling
    }
  }
  response_.erase(0, write_bytes);
  if (response_.empty() == true) {
    setToSend(false);
  }
}

bool Client::isErrorCode(void) {
  if (status_[0] == '2') {
    return true;
  }
  return false;
}

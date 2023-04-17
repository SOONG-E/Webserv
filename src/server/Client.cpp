#include "Client.hpp"

#include <cerrno>
#include <cstring>
#include <iostream>

Client::Client(const int fd, const TcpServer* tcp_server,
               const SocketAddress& address)
    : fd_(fd),
      tcp_server_(tcp_server),
      address_(address),
      is_response_ready(false) {}

Client::Client(const Client& origin)
    : fd_(origin.fd_),
      tcp_server_(origin.tcp_server_),
      address_(origin.address_),
      is_response_ready(origin.is_response_ready) {}

Client::~Client() {}

/*======================//
 Getter
/*======================*/

int Client::getFd() const { return fd_; }
HttpServer* Client::getHttpServer(void) const { return http_server_; }
HttpRequest Client::getRequest(void) const { return request_; }
std::string Client::getResponse(void) const { return response_; }

/*======================//
 Setter
/*======================*/

/*======================//
 process
/*======================*/

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
  request_.parse();
  if (request_.isCompleted() == true) {
    lookUpHttpServer();
    lookUpLocation();
    passRequestToHandler();
  }
  //
}

/* read data from socket */
std::string Client::readData(void) {
  char buffer[BUFFER_SIZE];
  std::size_t read_bytes = recv(fd_, &buffer, BUFFER_SIZE, 0);

  if (read_bytes == -1) {
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
  const TcpServer::HttpServerType virtual_servers =
      tcp_server_->getVirtualServers();
  TcpServer::HttpServerType::const_iterator server =
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

/* pass the request to eligible handler */
void Client::passRequestToHandler(void) {
  if (location_.isCgi() == true) {
    // cgi handler
    return;
  }
  if (location_.getAutoindex() == true) {
    // autoindex handle
    return;
  }
  // static handler
}

void Client::writeData(void) {}

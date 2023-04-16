#include "Client.hpp"

#include <cerrno>
#include <cstring>
#include <iostream>

Client::Client(const int fd, const TcpServer* tcp_server,
               const SocketAddress& address)
    : fd_(fd), tcp_server_(tcp_server), address_(address) {}

Client::Client(const Client& origin)
    : fd_(origin.fd_),
      tcp_server_(origin.tcp_server_),
      address_(origin.address_) {}

Client::~Client() {}

/*==========================*/
//         Getter           //
/*==========================*/

int Client::getFd() const { return fd_; }
HttpServer* Client::getHttpServer(void) const { return http_server_; }
HttpRequest Client::getRequest(void) const { return request_; }
std::string Client::getResponse(void) const { return response_; }

/*==========================*/
//         Setter           //
/*==========================*/

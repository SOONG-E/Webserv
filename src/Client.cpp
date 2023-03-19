#include "Client.hpp"

Client::Client(int socket, const SocketAddress& cli_addr,
               const SocketAddress& serv_addr)
    : socket_(socket), cli_address_(cli_addr), serv_address_(serv_addr) {}

Client::Client(const Client& src) { *this = src; }

Client::~Client() {}

Client& Client::operator=(const Client& src) {
  socket_ = src.socket_;
  cli_address_ = src.cli_address_;
  serv_address_ = src.serv_address_;
  parser_ = src.parser_;

  return *this;
}

int Client::getSocket() const { return socket_; }

HttpParser& Client::getParser() { return parser_; }

std::string Client::getKey() const {
  return serv_address_.getIP() + ":" + serv_address_.getPort();
}

HttpResponse& Client::getHttpResponse() { return response_; }

std::string Client::receive() const {
  char buf[BUF_SIZE] = {0};

  ssize_t read_bytes = recv(socket_, &buf, BUF_SIZE, 0);
  if (read_bytes == -1) throw SocketReceiveException(strerror(errno));

  return std::string(buf, static_cast<size_t>(read_bytes));
}

void Client::send(const ServerBlock* server_block) {
  std::string response = response_.generate(parser_.getRequest(), server_block);
  response = backup_ + response;
  size_t response_size = response.size();

  ssize_t write_bytes = ::send(socket_, response.c_str(), response_size, 0);

  if (write_bytes == -1) throw SocketSendException(strerror(errno));
  if (write_bytes < response_size) {
    backup_ = response.substr(write_bytes);
  }
}

Client::SocketReceiveException::SocketReceiveException(const char* cause)
    : cause(cause) {}

const char* Client::SocketReceiveException::what() const throw() {
  return cause;
}

Client::SocketSendException::SocketSendException(const char* cause)
    : cause(cause) {}

const char* Client::SocketSendException::what() const throw() { return cause; }
#include "Client.hpp"

#include <cerrno>

Client::Client(int socket, const SocketAddress& cli_addr,
               const SocketAddress& serv_addr)
    : socket_(socket),
      socket_key_(serv_addr.getIP() + ":" + serv_addr.getPort()),
      cli_address_(cli_addr),
      serv_address_(serv_addr) {}

Client::Client(const Client& src) { *this = src; }

Client::~Client() {}

Client& Client::operator=(const Client& src) {
  socket_ = src.socket_;
  socket_key_ = src.socket_key_;
  cli_address_ = src.cli_address_;
  serv_address_ = src.serv_address_;
  parser_ = src.parser_;

  return *this;
}

int Client::getSocket() const { return socket_; }

HttpParser& Client::getParser() { return parser_; }

const HttpParser& Client::getParser() const { return parser_; }

const std::string& Client::getSocketKey() const { return socket_key_; }

HttpResponse& Client::getResponseObj() { return response_obj_; }

const HttpResponse& Client::getResponseObj() const { return response_obj_; }

std::string Client::receive() const {
  char buf[BUF_SIZE] = {0};

  ssize_t read_bytes = recv(socket_, &buf, BUF_SIZE, 0);
  if (read_bytes == -1) throw SocketReceiveException(strerror(errno));

  return std::string(buf, static_cast<size_t>(read_bytes));
}

void Client::send(const ServerBlock* server_block) {
  std::string response;

  if (response_obj_.getBackup().empty()) {
    response = response_obj_.generate(parser_.getRequestObj(), server_block);
  } else {
    response = response_obj_.getBackup();
  }
  size_t response_size = response.size();

  size_t write_bytes = ::send(socket_, response.c_str(), response_size, 0);

  if (write_bytes == static_cast<size_t>(-1)) {
    throw SocketSendException(strerror(errno));
  }

  if (write_bytes < response_size) {
    response_obj_.setBackup(response.substr(write_bytes));
  }
}

bool Client::isPartialWritten() const {
  return !response_obj_.getBackup().empty();
}

Client::SocketReceiveException::SocketReceiveException(const char* cause)
    : cause(cause) {}

const char* Client::SocketReceiveException::what() const throw() {
  return cause;
}

Client::SocketSendException::SocketSendException(const char* cause)
    : cause(cause) {}

const char* Client::SocketSendException::what() const throw() { return cause; }
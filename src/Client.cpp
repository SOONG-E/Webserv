#include "Client.hpp"

Client::Client(int socket, const SocketAddress& cli_addr,
               const SocketAddress& serv_addr)
    : socket_(socket),
      cli_addr_(cli_addr),
      serv_addr_(serv_addr),
      server_block_(NULL) {}

Client::Client(const Client& src) { *this = src; }

Client::~Client() {}

Client& Client::operator=(const Client& src) {
  socket_ = src.socket_;
  cli_addr_ = src.cli_addr_;
  serv_addr_ = src.serv_addr_;
  parser_ = src.parser_;
  server_block_ = src.server_block_;

  return *this;
}

int Client::getSocket() const { return socket_; }

HttpParser& Client::getParser() { return parser_; }

std::string Client::getKey() const {
  return serv_addr_.getIP() + ":" + serv_addr_.getPort();
}

void Client::setServerBlock(const ServerBlock* server_block) {
  server_block_ = server_block;
}

std::string Client::receive() const {
  char buf[BUF_SIZE] = {0};

  ssize_t read_bytes = recv(socket_, &buf, BUF_SIZE, 0);
  if (read_bytes == -1) throw SocketReceiveException(strerror(errno));

  return std::string(buf, static_cast<size_t>(read_bytes));
}

Client::SocketReceiveException::SocketReceiveException(const char* cause)
    : cause(cause) {}

const char* Client::SocketReceiveException::what() const throw() {
  return cause;
}

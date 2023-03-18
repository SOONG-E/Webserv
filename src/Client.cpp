#include "Client.hpp"

Client::Client(int socket, const SocketAddress& cli_addr,
               const SocketAddress& serv_addr)
    : _socket(socket),
      _cli_addr(cli_addr),
      _serv_addr(serv_addr),
      _server_block(NULL) {}

Client::Client(const Client& src) { *this = src; }

Client::~Client() {}

Client& Client::operator=(const Client& src) {
  _socket = src._socket;
  _cli_addr = src._cli_addr;
  _serv_addr = src._serv_addr;
  _parser = src._parser;
  _server_block = src._server_block;

  return *this;
}

int Client::getSocket() const { return _socket; }

HttpParser& Client::getParser() { return _parser; }

std::string Client::getKey() const {
  return _serv_addr.getIP() + ":" + _serv_addr.getPort();
}

void Client::setServerBlock(const ServerBlock* server_block) {
  _server_block = server_block;
}

std::string Client::receive() const {
  char buf[BUF_SIZE] = {0};

  ssize_t read_bytes = recv(_socket, &buf, BUF_SIZE, 0);
  if (read_bytes == -1) throw SocketReceiveException(strerror(errno));

  return std::string(buf, static_cast<size_t>(read_bytes));
}

Client::SocketReceiveException::SocketReceiveException(const char* cause)
    : cause(cause) {}

const char* Client::SocketReceiveException::what() const throw() {
  return cause;
}

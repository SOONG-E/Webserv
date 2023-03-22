#include "Client.hpp"

Client::Client(int fd, const SocketAddress& cli_addr,
               const SocketAddress& serv_addr)
    : fd_(fd),
      socket_key_(serv_addr.getIP() + ":" + serv_addr.getPort()),
      cli_address_(cli_addr),
      serv_address_(serv_addr) {}

Client::Client(const Client& src) { *this = src; }

Client::~Client() {}

Client& Client::operator=(const Client& src) {
  fd_ = src.fd_;
  socket_key_ = src.socket_key_;
  cli_address_ = src.cli_address_;
  serv_address_ = src.serv_address_;
  parser_ = src.parser_;
  response_obj_ = src.response_obj_;
  response_buf_ = src.response_buf_;

  return *this;
}

int Client::getFD() const { return fd_; }

HttpParser& Client::getParser() { return parser_; }

const HttpParser& Client::getParser() const { return parser_; }

const std::string& Client::getSocketKey() const { return socket_key_; }

HttpResponse& Client::getResponseObj() { return response_obj_; }

const HttpResponse& Client::getResponseObj() const { return response_obj_; }

const std::string& Client::getRequestMethod() const {
  return parser_.getRequestObj().getMethod();
}
std::string Client::getRequestHeader(const std::string& target) const {
  return parser_.getRequestObj().getHeader(target);
}

void Client::setResponseStatus(const std::string& code,
                               const std::string& reason) {
  response_obj_.setStatus(code, reason);
}

void Client::appendRequest(const std::string& request) {
  parser_.appendRequest(request);
}

std::string Client::receive() const {
  char buf[BUF_SIZE] = {0};

  ssize_t read_bytes = recv(fd_, &buf, BUF_SIZE, 0);
  if (read_bytes == -1) throw SocketReceiveException(strerror(errno));

  return std::string(buf, static_cast<size_t>(read_bytes));
}

void Client::send(const ServerBlock* server_block) {
  if (!isPartialWritten())
    response_buf_ =
        response_obj_.generate(parser_.getRequestObj(), server_block);

  size_t write_bytes =
      ::send(fd_, response_buf_.c_str(), response_buf_.size(), 0);

  if (write_bytes == static_cast<size_t>(-1)) {
    throw SocketSendException(strerror(errno));
  }

  response_buf_.erase(0, write_bytes);
}

bool Client::isResponseSuccess() const { return response_obj_.isSuccess(); }

bool Client::isPartialWritten() const { return !response_buf_.empty(); }

bool Client::isParseCompleted() const { return parser_.isCompleted(); }

void Client::clearParser() { parser_.clear(); }

void Client::clearResponseBuf() { response_buf_.clear(); }

Client::SocketReceiveException::SocketReceiveException(const char* cause)
    : cause(cause) {}

const char* Client::SocketReceiveException::what() const throw() {
  return cause;
}

Client::SocketSendException::SocketSendException(const char* cause)
    : cause(cause) {}

const char* Client::SocketSendException::what() const throw() { return cause; }
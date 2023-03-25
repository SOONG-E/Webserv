#include "Client.hpp"

Client::Client(int fd, const SocketAddress& cli_addr,
               const SocketAddress& serv_addr)
    : fd_(fd), cli_address_(cli_addr), serv_address_(serv_addr) {
  logConnectionInfo();
}

Client::Client(const Client& src) { *this = src; }

Client::~Client() {}

Client& Client::operator=(const Client& src) {
  fd_ = src.fd_;
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

std::string Client::getSocketKey() const {
  return serv_address_.getIP() + ":" + serv_address_.getPort();
}

std::size_t Client::getClientMaxBodySize() const {
  return response_obj_.getLocationBlock()->body_limit;
}

std::string Client::getRequestMethod() const {
  return parser_.getRequestObj().getMethod();
}

std::size_t Client::getRequestContentLength() const {
  return parser_.getRequestObj().getContentLength();
}

std::string Client::getRequestHeader(const std::string& target) const {
  return parser_.getRequestObj().getHeader(target);
}

const ServerBlock* Client::getResponseServerBlock() const {
  return response_obj_.getServerBlock();
}

const LocationBlock* Client::getResponseLocationBlock() const {
  return response_obj_.getLocationBlock();
}

const std::string& Client::getRequestUri() const {
  return parser_.getRequestObj().getUri();
}

void Client::setResponseServerBlock(const ServerBlock* server_block) {
  response_obj_.setServerBlock(server_block);
}

void Client::setResponseLocationBlock(const LocationBlock* location_block) {
  response_obj_.setLocationBlock(location_block);
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
  if (read_bytes == -1) {
    throw SocketReceiveException(strerror(errno));
  }

  std::string ret(buf, static_cast<size_t>(read_bytes));
  if (read_bytes) {
    logReceiveInfo(ret);
  }

  return ret;
}

void Client::send() {
  if (!isPartialWritten())
    response_buf_ = response_obj_.generate(parser_.getRequestObj());

  size_t write_bytes =
      ::send(fd_, response_buf_.c_str(), response_buf_.size(), 0);

  if (write_bytes == static_cast<size_t>(-1)) {
    throw SocketSendException(strerror(errno));
  }

  Log::header("Send Information");
  logAddressInfo();
  std::cout << "[Send Data] " << '\n'
            << response_buf_.substr(0, write_bytes) << '\n';
  Log::footer();

  response_buf_.erase(0, write_bytes);
}

bool Client::isAllowedMethod() const {
  return response_obj_.isAllowedMethod(parser_.getRequestObj().getMethod());
}

bool Client::isErrorStatus() const { return !response_obj_.isSuccess(); }

bool Client::isPartialWritten() const { return !response_buf_.empty(); }

bool Client::isParseCompleted() const { return parser_.isCompleted(); }

void Client::clearParser() { parser_.clear(); }

void Client::clearResponseBuf() { response_buf_.clear(); }

void Client::logAddressInfo() const {
  std::cout << "[Client address]" << '\n'
            << cli_address_.getIP() << ":" << cli_address_.getPort() << '\n'
            << "[Server address]" << '\n'
            << serv_address_.getIP() << ":" << serv_address_.getPort() << '\n';
}

void Client::logConnectionInfo() const {
  Log::header("Connection Information");
  logAddressInfo();
  Log::footer();
}

void Client::logReceiveInfo(const std::string& request) const {
  Log::header("Receive Information");
  logAddressInfo();
  std::cout << "[Receive data]" << '\n' << request << '\n';
  Log::footer();
}

Client::SocketReceiveException::SocketReceiveException(const char* cause)
    : cause(cause) {}

const char* Client::SocketReceiveException::what() const throw() {
  return cause;
}

Client::SocketSendException::SocketSendException(const char* cause)
    : cause(cause) {}

const char* Client::SocketSendException::what() const throw() { return cause; }
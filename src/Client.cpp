#include "Client.hpp"

#include <cerrno>
#include <cstring>
#include <iostream>

#include "Log.hpp"
#include "constant.hpp"

Client::Client(int fd, const ServerBlock& default_server,
               const SocketAddress& cli_addr, const SocketAddress& serv_addr)
    : fd_(fd),
      cli_address_(cli_addr),
      serv_address_(serv_addr),
      response_obj_(default_server) {
  logConnectionInfo();
}

Client::Client(const Client& src)
    : fd_(src.fd_),
      cli_address_(src.cli_address_),
      serv_address_(src.serv_address_),
      parser_(src.parser_),
      response_obj_(src.response_obj_),
      cgi_(src.cgi_),
      buf_(src.buf_) {}

Client::~Client() {}

int Client::getFD() const { return fd_; }

HttpParser& Client::getParser() { return parser_; }

const HttpParser& Client::getParser() const { return parser_; }

std::string Client::getSocketKey() const {
  return serv_address_.getIP() + ":" + serv_address_.getPort();
}

const HttpRequest& Client::getRequestObj() const {
  return parser_.getRequestObj();
}

HttpResponse& Client::getResponseObj() { return response_obj_; }

const HttpResponse& Client::getResponseObj() const { return response_obj_; }

Cgi& Client::getCgi() { return cgi_; }

const Cgi& Client::getCgi() const { return cgi_; }

const SocketAddress& Client::getServerAddress() const { return serv_address_; }

const SocketAddress& Client::getClientAddress() const { return cli_address_; }

std::string Client::receive() const {
  char buf[BUF_SIZE];

  size_t read_bytes = recv(fd_, &buf, BUF_SIZE, 0);

  if (static_cast<ssize_t>(read_bytes) == -1) {
    throw SocketReceiveException(strerror(errno));
  }

  std::string request(buf, read_bytes);
  if (read_bytes) {
    logReceiveInfo(request);
  }
  return request;
}

void Client::send() {
  if (!isPartialWritten()) {
    buf_ = response_obj_.generate(parser_.getRequestObj(), isCgi(),
                                  cgi_.getCgiResponse());
  }

  std::size_t write_bytes = ::send(fd_, buf_.c_str(), buf_.size(), 0);
  if (write_bytes == static_cast<std::size_t>(-1)) {
    throw SocketSendException(strerror(errno));
  }

  Log::header("Send Information");
  logAddressInfo();
  std::cout << "[Send Data] " << '\n' << buf_.substr(0, write_bytes) << '\n';
  Log::footer("Send");

  buf_.erase(0, write_bytes);
}

void Client::executeCgiIO() {
  try {
    Selector cgi_selector;

    const int* pipe = cgi_.getPipe();

    cgi_selector.registerFD(pipe[WRITE]);
    cgi_selector.registerFD(pipe[READ]);

    if (cgi_selector.select() > 0) {
      if (!cgi_.isWriteCompleted() && cgi_selector.isWritable(pipe[WRITE])) {
        cgi_.writePipe();
      }
      if (cgi_.isWriteCompleted() && cgi_selector.isReadable(pipe[READ])) {
        cgi_.readPipe();
      }
    }
  } catch (const std::exception& e) {
    response_obj_.setStatus(C500);
    std::cerr << "[Error] Cgi IO failed: " << e.what() << '\n';
  }
}

void Client::clearBuffer() { buf_.clear(); }

void Client::clear() {
  parser_.clear();
  cgi_.clear();
  response_obj_.clear();
}

bool Client::isCgi() const {
  const LocationBlock* location_block = response_obj_.getLocationBlock();
  if (!location_block) return false;
  return !location_block->getCgiParam("CGI_PATH").empty();
}

bool Client::isPartialWritten() const { return !buf_.empty(); }

bool Client::isReadyToCgiIO() const {
  if (parser_.isCompleted() && isCgi() && !cgi_.isCompleted()) {
    return true;
  }
  return false;
}

bool Client::isReadyToSend() const {
  if (!response_obj_.isSuccessCode() ||
      (parser_.isCompleted() && (!isCgi() || cgi_.isCompleted()))) {
    return true;
  }
  return false;
}

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

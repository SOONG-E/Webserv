#include "Client.hpp"

#include <cerrno>
#include <cstring>
#include <iostream>

#include "Error.hpp"
#include "Log.hpp"
#include "constant.hpp"
#include "exception.hpp"

Client::Client(int fd, const ServerBlock& default_server,
               const SocketAddress& cli_addr, const SocketAddress& serv_addr)
    : fd_(fd),
      cli_address_(cli_addr),
      serv_address_(serv_addr),
      response_obj_(default_server) {
  setTimeout();
  // logConnectionInfo();
}

Client::Client(const Client& src)
    : fd_(src.fd_),
      cli_address_(src.cli_address_),
      serv_address_(src.serv_address_),
      parser_(src.parser_),
      response_obj_(src.response_obj_),
      cgi_(src.cgi_),
      buf_(src.buf_),
      timeout_(src.timeout_) {}

Client::~Client() {}

int Client::getFD() const { return fd_; }

HttpParser& Client::getParser() { return parser_; }

const HttpParser& Client::getParser() const { return parser_; }

std::string Client::getServerSocketKey() const {
  return serv_address_.getIP() + ":" + serv_address_.getPort();
}

int Client::getServerBlockKey() const {
  return response_obj_.getServerBlock()->getKey();
}

std::string Client::getSessionKey() const {
  const HttpRequest& request_obj = parser_.getRequestObj();

  return request_obj.getCookie("Session-ID") + ":" + cli_address_.getIP() +
         ":" + request_obj.getHeader("USER-AGENT");
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

std::time_t Client::getTimeout() const { return timeout_; }

void Client::setTimeout(std::time_t time) {
  timeout_ = time + KEEPALIVE_TIMEOUT;
}

void Client::setSessionTimeout() {
  Session* session = response_obj_.getSession();
  if (session) {
    session->setTimeout();
  }
}

std::string Client::receive() const {
  char buf[BUF_SIZE];

  std::size_t read_bytes = recv(fd_, &buf, BUF_SIZE, 0);

  if (read_bytes == ERROR<std::size_t>()) {
    // Error::log(Error::INFO[ERECV], std::strerror(errno));
    throw ConnectionClosedException();
  }
  if (read_bytes == 0) {
    throw ConnectionClosedException();
  }
  std::string request(buf, read_bytes);
  // logReceiveInfo(request);
  return request;
}

void Client::send() {
  if (!isPartialWritten()) {
    buf_ = response_obj_.generate(parser_.getRequestObj(), isCgi(),
                                  cgi_.getResponse());
  }
  std::size_t write_bytes = ::send(fd_, buf_.c_str(), buf_.size(), 0);

  if (write_bytes == ERROR<std::size_t>()) {
    // Error::log(Error::INFO[ESEND], std::strerror(errno));
    throw ConnectionClosedException();
  }

  // Log::header("Send Information");
  // logAddressInfo();
  // std::cout << "[Send Data] " << '\n' << buf_.substr(0, write_bytes) << '\n';
  // Log::footer("Send");

  buf_.erase(0, write_bytes);
}

void Client::runCgiProcess(Selector& selector) {
  const HttpRequest& request_obj = parser_.getRequestObj();

  cgi_.execute(request_obj, response_obj_, cli_address_, serv_address_,
               response_obj_.getLocationBlock()->getCgiParam("CGI_PATH"));

  selector.registerFD(cgi_.getReadFD());
  if (request_obj.getMethod() == "POST") {
    selector.registerFD(cgi_.getWriteFD());
  }
}

void Client::executeCgiIO(Selector& selector) {
  try {
    if (cgi_.hasBody() && selector.isWritable(cgi_.getWriteFD())) {
      cgi_.write(selector);
      setSessionTimeout();
    }
    if (selector.isReadable(cgi_.getReadFD())) {
      cgi_.read(selector);
      setSessionTimeout();
    }
  } catch (const ResponseException& e) {
    response_obj_.setStatus(e.status);
    // Error::log(Error::INFO[ECGI], e.what());
  }
}

void Client::closeConnection() const {
  close(fd_);

  // Log::header("Close Connection Information");
  // logAddressInfo();
  // Log::footer("Close Connection");
}

void Client::clear() {
  parser_.clear();
  cgi_.clear();
  response_obj_.clear();
}

bool Client::isCgi() const {
  const HttpRequest& request_obj = parser_.getRequestObj();
  if (request_obj.getMethod() == METHODS[DELETE]) {
    return false;
  }
  const LocationBlock* location_block = response_obj_.getLocationBlock();
  if (!location_block) {
    return false;
  }
  return location_block->isCgi(request_obj.getUri(), request_obj.getMethod(),
                               request_obj.getQueryString());
}

bool Client::isPartialWritten() const { return !buf_.empty(); }

bool Client::isReadyToCgiIO() const {
  if (parser_.isCompleted() && isCgi() && !cgi_.isCompleted() &&
      response_obj_.isSuccessCode()) {
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
  static_cast<void>(request);
  std::cout << "[Receive data]" << '\n' << request << '\n';
  Log::footer();
}

const char* Client::ConnectionClosedException::what() const throw() {
  return "";
}

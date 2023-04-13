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
    : fd_(fd), cli_address_(cli_addr), serv_address_(serv_addr) {
  this->setTimeout();
  // logConnectionInfo();
}

Client::Client(const Client& src)
    : fd_(src.fd_),
      cli_address_(src.cli_address_),
      serv_address_(src.serv_address_),
      cgi_(src.cgi_),
      buf_(src.buf_),
      timeout_(src.timeout_) {}

Client::~Client() {}

/*==========================*/
//         Getter           //
/*==========================*/

int Client::getFD() const { return fd_; }

std::string Client::getServerSocketKey() const {
  return serv_address_.getIP() + ":" + serv_address_.getPort();
}

int Client::getServerBlockKey() const {
  return getResponse().getServerBlock().getKey();
}

std::string Client::getSessionKey() const {
  return cli_address_.getIP() + ":" + request_.getHeader("USER-AGENT");
}

HttpRequest& Client::getRequest() { return request_; }

const HttpRequest& Client::getRequest() const { return request_; }

HttpResponse& Client::getResponse() { return response_; }

const HttpResponse& Client::getResponse() const { return response_; }

Cgi& Client::getCgi() { return cgi_; }

const Cgi& Client::getCgi() const { return cgi_; }

const SocketAddress& Client::getServerAddress() const { return serv_address_; }

const SocketAddress& Client::getClientAddress() const { return cli_address_; }

std::time_t Client::getTimeout() const { return timeout_; }

/*==========================*/
//         Setter           //
/*==========================*/

void Client::setTimeout(std::time_t time) {
  timeout_ = time + KEEPALIVE_TIMEOUT;
}

void Client::setSessionTimeout() {
  Session* session = getResponse().getSession();
  if (session) {
    session->setTimeout();
  }
}

/*==========================*/
//   interact with socket   //
/*==========================*/

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
  if (!isPartialResponse()) {
    buf_ = getResponse().generate(getRequest());
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

/*===========================*/
// run and interact with cgi //
/*===========================*/

void Client::runCgiProcess(Selector& selector) {
  const HttpRequest& request_obj = getRequest();

  cgi_.execute(request_obj, getResponse(), cli_address_, serv_address_);

  selector.registerFD(cgi_.getReadFD());
  if (request_obj.getMethod() == "POST") {
    selector.registerFD(cgi_.getWriteFD());
  }
}

void Client::executeCgiIO(Selector& selector) {
  try {
    if (cgi_.hasBody() && selector.isWritable(cgi_.getWriteFD())) {
      cgi_.write(selector);
    }
    if (selector.isReadable(cgi_.getReadFD())) {
      cgi_.read(selector, getResponse());
    } else if (cgi_.getTimeout() < std::time(NULL)) {
      cgi_.cleanUp(selector);
      throw ResponseException(C504);
    }
  } catch (const ResponseException& e) {
    getResponse().setStatus(e.status);
    // Error::log(Error::INFO[ECGI], e.what());
  }
  this->setTimeout();
  this->setSessionTimeout();
}

/*===========================*/
//         clear up          //
/*===========================*/

void Client::closeConnection() const {
  close(fd_);

  Session* session = getResponse().getSession();
  if (session) {
    session->setClient(NULL);
  }

  // Log::header("Close Connection Information");
  // logAddressInfo();
  // Log::footer("Close Connection");
}

void Client::clear() {
  cgi_.clear();
  getResponse().clear();
}

/*===========================*/
//      check condition      //
/*===========================*/

bool Client::isCgi() const {
  const HttpRequest& request = getRequest();
  if (request.getMethod() == METHODS[DELETE]) {
    return false;
  }
  const LocationBlock location_block = getResponse().getLocationBlock();
  return location_block.isCgi(request.getUri(), request.getMethod(),
                              request.getQueryString());
}

bool Client::isPartialResponse() const { return !buf_.empty(); }

bool Client::isReadyToCgiIO() const {
  if (getRequest().isCompleted() && isCgi() && !cgi_.isCompleted() &&
      getResponse().isSuccessCode()) {
    return true;
  }
  return false;
}

bool Client::isReadyToSend() const {
  if (!getResponse().isSuccessCode() ||
      (getRequest().isCompleted() && (!isCgi() || cgi_.isCompleted()))) {
    return true;
  }
  return false;
}

/*===========================*/
//            log            //
/*===========================*/

void Client::logAddressInfo() const {
  std::cout << "[Client address]" << '\n'
            << cli_address_.getIP() << ":" << cli_address_.getPort() << '\n'
            << "[Server address]" << '\n'
            << serv_address_.getIP() << ":" << serv_address_.getPort() << '\n';
}

void Client::logConnectionInfo() const {
  Log::header("Connection Information");
  this->logAddressInfo();
  Log::footer();
}

void Client::logReceiveInfo(const std::string& request) const {
  Log::header("Receive Information");
  this->logAddressInfo();
  std::cout << "[Receive data]" << '\n' << request << '\n';
  Log::footer();
}

const char* Client::ConnectionClosedException::what() const throw() {
  return "";
}

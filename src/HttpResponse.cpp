#include "HttpResponse.hpp"

#include <stdexcept>

#include "ResponseStatus.hpp"

const std::string HttpResponse::DEFAULTS[] = {"200", "html/error.html"};

HttpResponse::HttpResponse()
    : code_(DEFAULTS[CODE]),
      reason_(ResponseStatus::REASONS[C200]),
      server_block_(NULL),
      location_block_(NULL) {}

HttpResponse::HttpResponse(const HttpResponse& origin)
    : code_(origin.code_),
      reason_(origin.reason_),
      server_block_(origin.server_block_),
      location_block_(origin.location_block_) {}

HttpResponse& HttpResponse::operator=(const HttpResponse& origin) {
  if (this != &origin) {
    code_ = origin.code_;
    reason_ = origin.reason_;
    server_block_ = origin.server_block_;
    location_block_ = origin.location_block_;
  }
  return *this;
}

HttpResponse::~HttpResponse() {}

const ServerBlock* HttpResponse::getServerBlock(void) const {
  return server_block_;
}

const LocationBlock* HttpResponse::getLocationBlock(void) const {
  return location_block_;
}

void HttpResponse::setStatus(const std::string& code,
                             const std::string& reason) {
  code_ = code;
  reason_ = reason;
}

void HttpResponse::setServerBlock(const ServerBlock* server_block) {
  server_block_ = server_block;
}

void HttpResponse::setLocationBlock(const LocationBlock* location_block) {
  location_block_ = location_block;
}

bool HttpResponse::isAllowedMethod(std::string method) const {
  if (location_block_->allowed_methods.find(method) ==
      location_block_->allowed_methods.end()) {
    return false;
  }
  return true;
}

bool HttpResponse::isSuccess(void) const {
  return code_.size() == 3 && code_[0] == '2';
}

std::string HttpResponse::generate(const HttpRequest& request) {
  std::string body;
  if (!isSuccess()) {
    body = readFile(server_block_->getErrorPage(code_));
    return generateResponse(request, body);
  }
  try {
    body = rootUri(request.getUri());
  } catch (std::runtime_error& e) {
    code_ = "404";
    reason_ = ResponseStatus::REASONS[C404];
    body = readFile(server_block_->getErrorPage(code_));
  }
  return generateResponse(request, body);
}

std::string HttpResponse::generateResponse(const HttpRequest& request,
                                           const std::string& body) const {
  std::string header = "HTTP/1.1 " + code_ + " " + reason_ + CRLF;
  header += "Date: " + currentTime() + CRLF;
  header += "Server: Webserv" + CRLF;
  header += "Content-Length: " + toString(body.size()) + CRLF;
  header += "Content-Type: text/html" + CRLF;
  header += "Connection: ";
  if (request.getHeader("Connection").empty()) {
    header += "Keep-Alive" + DOUBLE_CRLF;
    return header + body;
  }
  header += request.getHeader("Connection") + DOUBLE_CRLF;
  return header + body;
}

std::string HttpResponse::currentTime(void) const {
  char buf[30];
  time_t timestamp = time(NULL);
  struct tm* time_info = localtime(&timestamp);
  strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", time_info);
  return std::string(buf);
}

std::string HttpResponse::rootUri(const std::string& request_uri) const {
  std::string filename =
      location_block_->root + request_uri.substr(request_uri.size());
  if (filename.back() == '/') {
    return readIndexFile(location_block_->index, filename);
  }
  return readFile(filename);
}

std::string HttpResponse::readIndexFile(const std::set<std::string>& index,
                                        const std::string& filename) const {
  for (std::set<std::string>::const_iterator iter = index.begin();
       iter != index.end(); ++iter) {
    try {
      return readFile(filename + *iter);
    } catch (std::runtime_error& e) {
      continue;
    }
  }
  throw FileOpenException();
}

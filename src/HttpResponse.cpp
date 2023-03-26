#include "HttpResponse.hpp"

#include "exception.hpp"

const std::string HttpResponse::DEFAULT_ERROR_PAGE = "html/error.html";

HttpResponse::HttpResponse(const ServerBlock& default_server)
    : code_(ResponseStatus::CODES[DEFAULT_INDEX]),
      reason_(ResponseStatus::REASONS[DEFAULT_INDEX]),
      default_server_(default_server),
      server_block_(NULL),
      location_block_(NULL) {}

HttpResponse::HttpResponse(const HttpResponse& origin)
    : code_(origin.code_),
      reason_(origin.reason_),
      default_server_(origin.default_server_),
      server_block_(origin.server_block_),
      location_block_(origin.location_block_) {}

HttpResponse::~HttpResponse() {}

const ServerBlock* HttpResponse::getServerBlock(void) const {
  return server_block_;
}

const LocationBlock* HttpResponse::getLocationBlock(void) const {
  return location_block_;
}

void HttpResponse::setStatus(const int index) {
  code_ = ResponseStatus::CODES[index];
  reason_ = ResponseStatus::REASONS[index];
}

void HttpResponse::setServerBlock(const ServerBlock* server_block) {
  server_block_ = server_block;
}

void HttpResponse::setLocationBlock(const LocationBlock* location_block) {
  location_block_ = location_block;
}

void HttpResponse::clear(void) {
  code_ = ResponseStatus::CODES[DEFAULT_INDEX];
  reason_ = ResponseStatus::REASONS[DEFAULT_INDEX];
  server_block_ = NULL;
  location_block_ = NULL;
}

bool HttpResponse::isSuccessCode(void) const {
  return code_.size() == 3 && code_[0] == '2';
}

std::string HttpResponse::generate(HttpRequest& request) {
  std::string body;
  if (!isSuccessCode()) {
    if (!server_block_) {
      server_block_ = &default_server_;
    }
    body = readFile(server_block_->getErrorPage(code_));
    return generateResponse(request, body);
  }
  try {
    body = rootUri(request.getUri());
  } catch (FileOpenException& e) {
    code_ = ResponseStatus::CODES[C404];
    reason_ = ResponseStatus::REASONS[C404];
    body = readFile(server_block_->getErrorPage(code_));
  }
  return generateResponse(request, body);
}

std::string HttpResponse::generateResponse(const HttpRequest& request,
                                           const std::string& body) const {
  if (request.getMethod() == METHODS[HEAD]) {
    return combine(request, "");
  }
  return combine(request, body);
}

std::string HttpResponse::combine(const HttpRequest& request,
                                  const std::string& body) const {
  std::string header = "HTTP/1.1 " + code_ + " " + reason_ + CRLF;
  header += "Date: " + currentTime() + CRLF;
  header += "Server: Webserv" + CRLF;
  header += "Content-Length: " + toString(body.size()) + CRLF;
  header += "Content-Type: text/html" + CRLF;
  header += "Connection: ";
  if (request.getHeader("Connection").empty()) {
    header += "keep-alive" + DOUBLE_CRLF;
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

std::string HttpResponse::rootUri(std::string& request_uri) const {
  std::string root = location_block_->getRoot();
  if (*root.rbegin() != '/') {
    root += '/';
  }
  std::string filename =
      request_uri.replace(0, location_block_->getUri().size(), root);
  if (*filename.rbegin() == '/') {
    return readIndexFile(filename, location_block_->getIndex());
  }
  return readFile(filename);
}

std::string HttpResponse::readIndexFile(
    const std::string& filename, const std::set<std::string>& index) const {
  for (std::set<std::string>::const_iterator iter = index.begin();
       iter != index.end(); ++iter) {
    try {
      return readFile(filename + *iter);
    } catch (FileOpenException& e) {
      continue;
    }
  }
  throw FileOpenException();
}

#include "HttpResponse.hpp"

#include <stdexcept>

#include "ResponseStatus.hpp"

const std::string HttpResponse::DEFAULTS[] = {"200", "page/error.html"};

HttpResponse::HttpResponse()
    : code_(DEFAULTS[CODE]), reason_(ResponseStatus::REASONS[C200]) {}

HttpResponse::HttpResponse(const HttpResponse& origin)
    : code_(origin.code_), reason_(origin.reason_) {}

HttpResponse& HttpResponse::operator=(const HttpResponse& origin) {
  if (this != &origin) {
    code_ = origin.code_;
    reason_ = origin.reason_;
  }
  return *this;
}

HttpResponse::~HttpResponse() {}

void HttpResponse::setStatus(const std::string& code,
                             const std::string& reason) {
  code_ = code;
  reason_ = reason;
}

bool HttpResponse::isSuccess(void) const {
  return code_.size() == 3 && code_[0] == '2';
}

std::string HttpResponse::generate(const HttpRequest& request,
                                   const ServerBlock* server_block) {
  std::string body;
  if (!isSuccess()) {
    body = readFile(server_block->getErrorPage(code_));
    return generateResponse(body);
  }
  try {
    body = rootUri(request.getUri(), server_block->getLocationBlocks());
  } catch (std::runtime_error& e) {
    code_ = "404";
    reason_ = ResponseStatus::REASONS[C404];
    body = readFile(server_block->getErrorPage(code_));
  }
  return generateResponse(body);
}

std::string HttpResponse::currentTime(void) const {
  char buf[30];
  time_t timestamp = time(NULL);
  struct tm* time_info = localtime(&timestamp);
  strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", time_info);
  return std::string(buf);
}

std::string HttpResponse::generateResponse(const std::string& body) const {
  std::string header = "HTTP/1.1 " + code_ + " " + reason_ + CRLF;
  header += "Date: " + currentTime() + CRLF;
  header += "Server: Webserv" + CRLF;
  header += "Content-Length: " + toString(body.size()) + CRLF;
  header += "Content-Type: text/html" + CRLF;
  header += "Connection: Keep-Alive" + DOUBLE_CRLF;
  return header + body;
}

std::string HttpResponse::rootUri(
    const std::string& request_uri,
    const std::vector<LocationBlock>& locations) const {
  std::size_t end_pos = -1;
  while (true) {
    end_pos = request_uri.find("/", end_pos + 1);
    if (end_pos == std::string::npos) throw std::runtime_error("");
    std::string rooted_uri = request_uri.substr(0, end_pos + 1);
    for (std::size_t i = 0; i < locations.size(); ++i) {
      if (rooted_uri == locations[i].uri) {
        return readFile(locations[i].root + request_uri.substr(end_pos + 1));
      }
    }
  }
}

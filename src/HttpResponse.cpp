#include "HttpResponse.hpp"

#include "ResponseStatus.hpp"

HttpResponse::HttpResponse()
    : code_(DEFAULT_CODE), reason_(ReponseStatus::REASONS[C200]) {}

HttpResponse::HttpResponse(const HttpResponse& origin) {}

HttpResponse& HttpResponse::operator=(const HttpResponse& origin) {}

HttpResponse::~HttpResponse() {}

const std::string& HttpResponse::getCode(void) const { return code_; }

void HttpResponse::setCode(const std::string& code) { code_ = code; }

void HttpResponse::setReason(const std::string& reason) { reason_ = reason; }

void HttpResponse::generateHeader() {
  header_ = "HTTP/1.1 " + code_ + " " + reason_ + CRLF;
  header_ += "Date: " + getDate() + CRLF;
  header_ += "Server: Webserv" + CRLF;
  header_ += "Content-Length: " + toString(body_.size()) + CRLF;
  header_ += "Content-Type: text/html" + CRLF;
  header_ += "Connection: Keep-Alive" + DOUBLE_CRLF;
}

std::string HttpResponse::getDate() const {
  char buf[30];
  time_t timestamp = time(NULL);
  struct tm* time_info = localtime(&timestamp);
  strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", time_info);
  return std::string(buf);
}

std::string HttpResponse::generate(const HttpRequest& request,
                                   const ServerBlock* server_block) {
  if (code_ != DEFAULT_CODE) {
    body_ = readFile(DEFAULTS[ERROR_PAGE]);
    generateHeader();
    return header_ + body_;
  }
  const std::vector<LocationBlock>& locations =
      server_block->getLocationBlocks();
  std::size_t location_end = request.getUri().rfind("/");
  std::string location = request.getUri().substr(0, location_end + 1);
  for (std::size_t i = 0; i < locations.size(); ++i) {
    if (location == locations[i].uri) {
      body_ = readFile(locations[i].root + location);
      break;
    }
  }
  if (body_.empty()) {
    body_ = readFile(DEFAULTS[ERROR_PAGE]);
    code_ = "404";
    reason_ = ReponseStatus::REASONS[C404];
  }
  generateHeader();
  return header_ + body_;
}

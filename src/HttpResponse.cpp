#include "HttpResponse.hpp"

HttpResponse::HttpResponse() : code_("200"), reason_("OK") {}

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
  if (code_ != "200") {
    body_ = readFile(kDefaults[kErrorPage]);
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
    body_ = readFile(kDefaults[kErrorPage]);
    code_ = "404";
    reason_ = "Not Found";
  }
  generateHeader();
  return header_ + body_;
}

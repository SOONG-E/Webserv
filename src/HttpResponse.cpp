#include "HttpResponse.hpp"

#include "ResponseStatus.hpp"

const std::string HttpResponse::DEFAULTS[] = {"200", "page/error.html"};

HttpResponse::HttpResponse()
    : code_(DEFAULTS[CODE]), reason_(ResponseStatus::REASONS[C200]) {}

HttpResponse::HttpResponse(const HttpResponse& origin)
    : code_(origin.code_),
      reason_(origin.reason_),
      header_(origin.header_),
      body_(origin.body_) {}

HttpResponse& HttpResponse::operator=(const HttpResponse& origin) {
  if (this != &origin) {
    code_ = origin.code_;
    reason_ = origin.reason_;
    header_ = origin.header_;
    body_ = origin.body_;
  }
  return *this;
}

HttpResponse::~HttpResponse() {}

const std::string& HttpResponse::getCode(void) const { return code_; }

void HttpResponse::setCode(const std::string& code) { code_ = code; }

void HttpResponse::setReason(const std::string& reason) { reason_ = reason; }

std::string HttpResponse::getDate(void) const {
  char buf[30];
  time_t timestamp = time(NULL);
  struct tm* time_info = localtime(&timestamp);
  strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", time_info);
  return std::string(buf);
}

void HttpResponse::generateHeader(void) {
  header_ = "HTTP/1.1 " + code_ + " " + reason_ + CRLF;
  header_ += "Date: " + getDate() + CRLF;
  header_ += "Server: Webserv" + CRLF;
  header_ += "Content-Length: " + toString(body_.size()) + CRLF;
  header_ += "Content-Type: text/html" + CRLF;
  header_ += "Connection: Keep-Alive" + DOUBLE_CRLF;
}

void HttpResponse::rootUri(const std::string& request_uri,
                           const std::vector<LocationBlock>& locations) {
  std::size_t end_pos = -1;
  do {
    end_pos = request_uri.find("/", end_pos + 1);
    if (end_pos == std::string::npos) throw;
    std::string rooted_uri = request_uri.substr(0, end_pos + 1);
    for (std::size_t i = 0; i < locations.size(); ++i) {
      if (rooted_uri == locations[i].uri) {
        body_ = readFile(locations[i].root + request_uri.substr(end_pos + 1));
        return;
      }
    }
  } while (body_.empty());
}

std::string HttpResponse::generate(const HttpRequest& request,
                                   const ServerBlock* server_block) {
  if (code_ != DEFAULTS[CODE]) {
    body_ = readFile(DEFAULTS[ERROR_PAGE]);
    generateHeader();
    return header_ + body_;
  }
  try {
    rootUri(request.getUri(), server_block->getLocationBlocks());
  } catch (...) {
    body_ = readFile(DEFAULTS[ERROR_PAGE]);
    code_ = "404";
    reason_ = ResponseStatus::REASONS[C404];
  }
  generateHeader();
  return header_ + body_;
}

#include "HttpResponse.hpp"

#include "ResponseStatus.hpp"

const std::string HttpResponse::DEFAULTS[] = {"200", "page/error.html"};

HttpResponse::HttpResponse()
    : code_(DEFAULTS[CODE]), reason_(ResponseStatus::REASONS[C200]) {}

HttpResponse::HttpResponse(const HttpResponse& origin)
    : code_(origin.code_), reason_(origin.reason_), backup_(origin.backup_) {}

HttpResponse& HttpResponse::operator=(const HttpResponse& origin) {
  if (this != &origin) {
    code_ = origin.code_;
    reason_ = origin.reason_;
    backup_ = origin.backup_;
  }
  return *this;
}

HttpResponse::~HttpResponse() {}

const std::string& HttpResponse::getCode(void) const { return code_; }

const std::string& HttpResponse::getBackup(void) const { return backup_; }

void HttpResponse::setStatus(const std::string& code,
                             const std::string& reason) {
  code_ = code;
  reason_ = reason;
}

void HttpResponse::setBackup(const std::string& backup) { backup_ = backup; }
std::string HttpResponse::getDate(void) const {
  char buf[30];
  time_t timestamp = time(NULL);
  struct tm* time_info = localtime(&timestamp);
  strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", time_info);
  return std::string(buf);
}

std::string HttpResponse::generateHeader(const std::string& body) const {
  std::string header = "HTTP/1.1 " + code_ + " " + reason_ + CRLF;
  header += "Date: " + getDate() + CRLF;
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
    if (end_pos == std::string::npos) throw;
    std::string rooted_uri = request_uri.substr(0, end_pos + 1);
    for (std::size_t i = 0; i < locations.size(); ++i) {
      if (rooted_uri == locations[i].uri) {
        return readFile(locations[i].root + request_uri.substr(end_pos + 1));
      }
    }
  }
}

std::string HttpResponse::generate(const HttpRequest& request,
                                   const ServerBlock* server_block) {
  std::string body;
  if (code_ != DEFAULTS[CODE]) {
    body = readFile(DEFAULTS[ERROR_PAGE]);
    return generateHeader(body);
  }
  try {
    body = rootUri(request.getUri(), server_block->getLocationBlocks());
  } catch (...) {
    body =
        readFile(DEFAULTS[ERROR_PAGE]);  // server block은 있는데 location
                                         // 못찾았을 때 404 페이지 지정해준 거로
    code_ = "404";
    reason_ = ResponseStatus::REASONS[C404];
  }
  return generateHeader(body);
}

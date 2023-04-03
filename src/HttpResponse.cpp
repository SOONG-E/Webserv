#include "HttpResponse.hpp"

#include <dirent.h>
#include <sys/stat.h>

#include <cerrno>
#include <cstring>
#include <set>

#include "DirectoryListingHtml.hpp"
#include "File.hpp"
#include "ResponseStatus.hpp"
#include "constant.hpp"
#include "exception.hpp"
#include "utility.hpp"

const int HttpResponse::DEFAULT_STATUS = C200;
const std::string HttpResponse::DEFAULT_ERROR_PAGE = "html/error.html";

HttpResponse::HttpResponse(const ServerBlock& default_server)
    : status_(DEFAULT_STATUS),
      session_(NULL),
      default_server_(default_server),
      server_block_(NULL),
      location_block_(NULL) {}

HttpResponse::HttpResponse(const HttpResponse& origin)
    : status_(origin.status_),
      session_(origin.session_),
      default_server_(origin.default_server_),
      server_block_(origin.server_block_),
      location_block_(origin.location_block_) {}

HttpResponse::~HttpResponse() {}

Session* HttpResponse::getSession(void) const { return session_; }

const ServerBlock* HttpResponse::getServerBlock(void) const {
  return server_block_;
}

const LocationBlock* HttpResponse::getLocationBlock(void) const {
  return location_block_;
}

void HttpResponse::setStatus(const int status) { status_ = status; }

void HttpResponse::setSession(Session* session) { session_ = session; }

void HttpResponse::setServerBlock(const ServerBlock* server_block) {
  server_block_ = server_block;
}

void HttpResponse::setLocationBlock(const LocationBlock* location_block) {
  location_block_ = location_block;
}

void HttpResponse::clear(void) {
  status_ = DEFAULT_STATUS;
  server_block_ = NULL;
  location_block_ = NULL;
}

bool HttpResponse::isSuccessCode(void) const { return status_ <= C204; }

std::string HttpResponse::generate(const HttpRequest& request, bool is_cgi,
                                   const std::string& cgi_response) {
  if (!isSuccessCode()) {
    return generateErrorPage(request);
  }
  if (is_cgi) {
    return generateFromCgi(request, cgi_response);
  }
  if (request.getMethod() == METHODS[DELETE]) {
    return commonHeader(request) + CRLF;
  }
  std::string body;
  try {
    body = rootUri(request.getUri());
  } catch (FileOpenException& e) {
    setStatus(C404);
    body =
        readFile(server_block_->getErrorPage(ResponseStatus::CODES[status_]));
  }
  return generateResponse(request, body);
}

std::string HttpResponse::generateErrorPage(const HttpRequest& request) {
  if (!server_block_) {
    server_block_ = &default_server_;
  }
  std::string body =
      readFile(server_block_->getErrorPage(ResponseStatus::CODES[status_]));
  // test
  if (request.getMethod() == METHODS[PUT]) {
    setStatus(C200);
  }
  //
  return generateResponse(request, body);
}

std::string HttpResponse::generateResponse(const HttpRequest& request,
                                           const std::string& body) const {
  if (request.getMethod() == METHODS[HEAD]) {
    return combine(request, "");
  }
  return combine(request, body);
}

std::string HttpResponse::generateFromCgi(const HttpRequest& request,
                                          std::string cgi_response) const {
  std::string response = commonHeader(request);
  const std::string STATUS = "Status: ";
  std::size_t pos = cgi_response.find(STATUS);
  if (pos != std::string::npos) {
    std::size_t count = cgi_response.find(CRLF, pos) - pos;
    std::string status_line = cgi_response.substr(pos, count);
    cgi_response.erase(pos, count + CRLF.size());
    status_line.replace(0, STATUS.size(), "HTTP/1.1 ");
    response.replace(0, response.find(CRLF), status_line);
  }
  response += cgi_response;
  std::size_t bound_pos = response.find(DOUBLE_CRLF);
  if (bound_pos == std::string::npos) {
    return response + CRLF;
  }
  // entity-header
  bound_pos += CRLF.size();
  std::size_t body_size = response.size() - bound_pos - CRLF.size();
  response.insert(bound_pos, "Content-Length: " + toString(body_size) + CRLF);
  response.insert(bound_pos, "Content-Type: text/html" + CRLF);
  return response;
}

std::string HttpResponse::combine(const HttpRequest& request,
                                  const std::string& body) const {
  std::string header = commonHeader(request);
  // entity-header
  header += "Content-Length: " + toString(body.size()) + CRLF;
  header += "Content-Type: text/html" + DOUBLE_CRLF;
  return header + body;
}

std::string HttpResponse::commonHeader(const HttpRequest& request) const {
  // Status-Line
  std::string header = "HTTP/1.1 " + ResponseStatus::CODES[status_] + " " +
                       ResponseStatus::REASONS[status_] + CRLF;
  // general-header
  header += "Connection: ";
  if (!isSuccessCode()) {
    header += "close" + CRLF;
  } else if (!request.getHeader("CONNECTION").empty()) {
    header += request.getHeader("CONNECTION") + CRLF;
  } else {
    header += "keep-alive" + CRLF;
  }
  header += "Date: " + formatTime("%a, %d %b %Y %H:%M:%S GMT") + CRLF;
  if (!request.getHeader("Transfer-Encoding").empty()) {
    header +=
        "Transfer-Encoding: " + request.getHeader("Transfer-Encoding") + CRLF;
  }
  // response-header
  header += "Server: Webserv" + CRLF;
  if (session_ && (request.getCookie("Session-ID").empty() ||
                   request.getCookie("Session-ID") != session_->getID())) {
    header += "Set-Cookie: Session-ID=" + session_->getID() +
              "; Max-Age=" + COOKIE_MAX_AGE + CRLF;
  }
  return header;
}

std::string HttpResponse::rootUri(std::string uri) const {
  std::string root = location_block_->getRoot();
  if (*root.rbegin() != '/') {
    root += '/';
  }
  uri.replace(0, location_block_->getUri().size(), root);
  if (isDirectory(uri)) {
    uri = (*uri.rbegin() == '/') ? uri : uri + '/';
    return readIndexFile(uri);
  }
  return readFile(uri);
}

std::string HttpResponse::readIndexFile(const std::string& url) const {
  for (std::size_t i = 0; i < location_block_->getIndex().size(); ++i) {
    try {
      return readFile(url + location_block_->getIndex()[i]);
    } catch (FileOpenException& e) {
      continue;
    }
  }
  if (location_block_->getAutoindex() == false) {
    throw FileOpenException();
  }
  return directoryListing(url);
}

std::string HttpResponse::directoryListing(const std::string& url) const {
  DIR* dir = opendir(url.c_str());
  if (!dir) {
    throw FileOpenException(strerror(errno));
  }
  std::set<File> entries;
  struct dirent* entry;
  while ((entry = readdir(dir))) {
    std::string name = entry->d_name;
    if (name == ".") continue;
    struct stat statbuf;
    if (stat((url + name).c_str(), &statbuf) < 0) {
      throw FileOpenException(strerror(errno));
    }
    File file = {name, statbuf.st_mtime, statbuf.st_size};
    entries.insert(file);
  }
  closedir(dir);
  return DirectoryListingHtml::generate(url, entries);
}

#include "HttpParser.hpp"

#include <algorithm>
#include <stdexcept>

#include "Error.hpp"
#include "ResponseStatus.hpp"
#include "constant.hpp"
#include "exception.hpp"
#include "utility.hpp"

const std::size_t HttpParser::HEADER_MAX_SIZE = 8192;

HttpParser::HttpParser(const std::string& socket_buffer)
    : buffer_(socket_buffer), bound_pos_(std::string::npos) {}

HttpParser::HttpParser(const HttpParser& origin)
    : request_(origin.request_),
      buffer_(origin.buffer_),
      bound_pos_(origin.bound_pos_) {}

HttpParser& HttpParser::operator=(const HttpParser& origin) {
  if (this != &origin) {
    request_ = origin.request_;
    buffer_ = origin.buffer_;
    bound_pos_ = origin.bound_pos_;
  }
  return *this;
}

HttpParser::~HttpParser() {}

std::size_t HttpParser::getContentLength(void) {
  return request_.getContentLength();
}

HttpRequest& HttpParser::getRequestObj(void) { return request_; }

const HttpRequest& HttpParser::getRequestObj(void) const { return request_; }

const std::string& HttpParser::getBuffer(void) const { return buffer_; }

void HttpParser::appendRequest(const std::string& socket_buffer) {
  buffer_ += socket_buffer;
  if (!isHeaderSet()) {
    setHeader();
  }
  if (request_.getMethod() == METHODS[POST]) {
    handlePost();
  }
}

bool HttpParser::isCompleted(void) const {
  if (!request_.getMethod().empty() && request_.getMethod() != METHODS[POST])
    return true;
  return request_.getBody().size() == request_.getContentLength();
}

void HttpParser::clear(void) { *this = HttpParser(); }

bool HttpParser::isHeaderSet(void) const {
  return bound_pos_ != std::string::npos;
}

void HttpParser::setHeader(void) {
  bound_pos_ = buffer_.find(DOUBLE_CRLF);
  if (buffer_.size() <= HEADER_MAX_SIZE && bound_pos_ == std::string::npos) {
    return;
  }
  if (bound_pos_ > HEADER_MAX_SIZE) {
    throw ResponseException(C413);
  }
  parseHeader(buffer_.substr(0, bound_pos_));
  bound_pos_ += DOUBLE_CRLF.size();
}

void HttpParser::handlePost(void) {
  if (!request_.getHeader("CONTENT-LENGTH").empty()) {
    if (request_.getContentLength() == static_cast<std::size_t>(-1)) {
      try {
        request_.setContentLength(::stoi(request_.getHeader("CONTENT-LENGTH")));
      } catch (std::invalid_argument& e) {
        Error::log(e.what());
        throw ResponseException(C400);
      }
    }
    std::size_t content_length = request_.getContentLength();
    if (content_length < buffer_.size() - bound_pos_) {
      throw ResponseException(C413);
    }
    if (content_length == buffer_.size() - bound_pos_) {
      request_.setBody(buffer_.substr(bound_pos_, content_length));
    }
    return;
  }
  if (request_.getHeader("TRANSFER-ENCODING") != "chunked") {
    throw ResponseException(C411);
  }
  if (buffer_.find(DOUBLE_CRLF, bound_pos_) == std::string::npos) return;
  unchunkMessage(buffer_.substr(bound_pos_));
}

void HttpParser::parseHeader(const std::string& header_part) {
  std::size_t boundary = header_part.find(CRLF);
  std::string request_line = header_part.substr(0, boundary);
  parseRequestLine(request_line);
  std::string headers = header_part.substr(boundary + CRLF.length());
  parseHeaderFields(headers);
}

void HttpParser::parseQueryString(void) {
  const std::string& uri = request_.getUri();
  std::size_t query_boundary = uri.find("?");
  if (uri == "?" || query_boundary == std::string::npos) {
    return;
  }
  request_.setUri(uri.substr(0, query_boundary));
  if (request_.getMethod() == METHODS[GET] ||
      request_.getMethod() == METHODS[HEAD]) {
    request_.setQueryString(uri.substr(query_boundary));
  }
}

void HttpParser::parseRequestLine(const std::string& request_line) {
  std::vector<std::string> request_words = split(request_line);
  if (request_words.size() < 3 || request_words[2].length() < 6 ||
      request_words[2].substr(0, 4) != "HTTP") {
    throw ResponseException(C400);
  }
  if (request_words[2].substr(0, 5) != "HTTP/" ||
      request_words[2].substr(5) != "1.1") {
    throw ResponseException(C505);
  }
  request_.setMethod(request_words[0]);
  request_.setUri(request_words[1]);
  parseQueryString();
}

void HttpParser::parseCookie(void) {
  HttpRequest::cookie_list_type request_cookie_list;

  std::string cookies = request_.getHeader("COOKIE");
  std::vector<std::string> cookie_list = split(cookies, ";");
  std::vector<std::string> values;

  for (std::vector<std::string>::iterator it = cookie_list.begin();
       it != cookie_list.end(); ++it) {
    values = split(*it, "=");
    if (values.size() != 2) {
      throw ResponseException(C400);
    }
    request_cookie_list[trim(values[0])] = trim(values[1]);
  }
  request_.setCookie(request_cookie_list);
}

void HttpParser::parseHeaderFields(const std::string& header_part) {
  std::vector<std::string> headers = splitByCRLF(header_part);
  std::vector<std::string> line;
  for (std::vector<std::string>::const_iterator header = headers.begin();
       header != headers.end(); ++header) {
    if (header->length() == 0) break;
    line = split(*header, ":");
    if (line.size() < 2) throw ResponseException(C400);
    std::transform(line[0].begin(), line[0].end(), line[0].begin(), ::toupper);
    request_.addHeader(trim(line[0]), trim(line[1]));
  }

  request_.setHost(request_.getHeader("HOST"));
  if (request_.getHost().empty()) throw ResponseException(C400);
  if (request_.getHeader("CONTENT-TYPE").empty()) {
    request_.addHeader("CONTENT-TYPE", "application/octet-stream");
  }
  if (!request_.getHeader("COOKIE").empty()) {
    parseCookie();
  }
}

void HttpParser::unchunkMessage(const std::string& body_part) {
  std::string content;
  std::size_t content_length = 0;
  std::size_t chunk_size = 0;
  std::vector<std::string> chunks = splitByCRLF(body_part);
  if (chunks.size() < 1) throw ResponseException(C400);
  chunk_size = hexToInt(chunks[0]);
  std::size_t idx = 1;
  while (chunk_size && idx + 1 < chunks.size()) {
    content_length += chunk_size;
    if (chunks[idx].length() != chunk_size) throw ResponseException(C400);
    content += chunks[idx++];
    chunk_size = hexToInt(chunks[idx++]);
  }
  request_.setContentLength(content_length);
  request_.setBody(content);
}

std::vector<std::string> HttpParser::splitByCRLF(const std::string& content) {
  std::vector<std::string> substrings;
  std::size_t start = 0;
  std::size_t end;
  while (start != content.size()) {
    end = content.find(CRLF, start);
    if (end == std::string::npos) {
      substrings.push_back(content.substr(start));
      break;
    }
    substrings.push_back(content.substr(start, end - start));
    start = end + CRLF.length();
  }
  return substrings;
}

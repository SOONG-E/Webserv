#include "HttpParser.hpp"

#include <iostream>

#include "constant.hpp"
#include "exception.hpp"

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
  if (request_.getMethod() == "POST") {
    handlePost();
  }
}

bool HttpParser::isCompleted(void) const {
  if (!request_.getMethod().empty() && request_.getMethod() != "POST")
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
  request_ = HttpParser::parseHeader(buffer_.substr(0, bound_pos_));
  bound_pos_ += DOUBLE_CRLF.size();
}

void HttpParser::handlePost(void) {
  if (!request_.getHeader("Content-Length").empty()) {
    if (request_.getContentLength() == static_cast<std::size_t>(-1))
      request_.setContentLength(::stoi(request_.getHeader("Content-Length")));
    std::size_t content_length = request_.getContentLength();
    if (content_length < buffer_.size() - bound_pos_) {
      throw ResponseException(C413);
    }
    if (content_length == buffer_.size() - bound_pos_) {
      request_.setBody(buffer_.substr(bound_pos_, content_length));
    }
    return;
  }
  if (request_.getHeader("Transfer-Encoding") != "chunked") {
    throw ResponseException(C411);
  }
  if (buffer_.find(DOUBLE_CRLF, bound_pos_) == std::string::npos) return;
  unchunkMessage(request_, buffer_.substr(bound_pos_));
}

HttpRequest HttpParser::parseHeader(const std::string& request) {
  HttpRequest request2;
  std::size_t boundary = request.find(CRLF);
  std::string request_line = request.substr(0, boundary);
  parseRequestLine(request2, request_line);
  std::string headers = request.substr(boundary + CRLF.length());
  parseHeaders(request2, headers);

  return request2;
}

void HttpParser::parseRequestLine(HttpRequest& request_,
                                  const std::string& request) {
  std::vector<std::string> request_line = split(request);

  if (request_line.size() < 3 || request_line[2].length() < 6 ||
      request_line[2].substr(0, 4) != "HTTP") {
    throw ResponseException(C400);
  }
  if (request_line[2].substr(0, 5) != "HTTP/" ||
      request_line[2].substr(5) != "1.1") {
    throw ResponseException(C505);
  }
  request_.setMethod(request_line[0]);
  request_.setUri(request_line[1]);
}

void HttpParser::parseHeaders(HttpRequest& request_, std::string header_part) {
  std::vector<std::string> headers = splitByCRLF(header_part);
  std::vector<std::string> line;
  for (std::vector<std::string>::iterator header = headers.begin();
       header != headers.end(); ++header) {
    if (header->length() == 0) break;
    line = split(*header, ":");
    if (line.size() < 2) throw ResponseException(C400);
    request_.addHeader(trim(line[0]), trim(line[1]));
  }
  request_.setHost(request_.getHeader("Host"));
  if (request_.getHost().empty()) throw ResponseException(C400);
  if (request_.getHeader("Content-Type").empty())
    request_.addHeader("Content-Type", "application/octet-stream");
}

void HttpParser::unchunkMessage(HttpRequest& request_, std::string body) {
  std::string content;
  std::size_t content_length = 0;
  std::size_t chunk_size = 0;

  std::vector<std::string> chunks = splitByCRLF(body);
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

#include <exception>
#include <iostream>

#include "HttpParser.hpp"
#include "constant.hpp"

HttpParser::HttpParser(const std::string& socket_buffer)
    : buffer_(socket_buffer), bound_pos_(0) {}

HttpParser::HttpParser(const HttpParser& origin)
    : request_(origin.request_), buffer_(origin.buffer_), bound_pos_(origin.bound_pos_) {}

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

const HttpRequest& HttpParser::getRequest(void) const { return request_; }

const std::string& HttpParser::getBuffer(void) const { return buffer_; }

void HttpParser::appendBuffer(const std::string& socket_buffer) {
  buffer_ += socket_buffer;
  if (bound_pos_ == 0) {
    setHeader();
  }
  handlePost();
}

void HttpParser::setHeader(void) {
  bound_pos_ = buffer_.find(DOUBLE_CRLF);
  if (bound_pos_ == std::string::npos || bound_pos_ > HEADER_MAX_SIZE) throw;  // 413 ERROR
  request_ = HttpParser::parseHeader(buffer_.substr(0, bound_pos_ + 1));
  bound_pos_ += DOUBLE_CRLF.size();
}

void HttpParser::handlePost(void) {
  if (!request_.getHeader("Content-Length").empty()) {
    std::size_t content_length = ::stoi(request_.getHeader("Content-Length"));
    request_.setContentLength(content_length);
    if (buffer_.size() - bound_pos_ < content_length) return;
    request_.setBody(buffer_.substr(bound_pos_, content_length));
    return;
  }
  if (request_.getHeader("Transfer-Encoding") != "chunked") {
    throw;
  }
  if (buffer_.find(DOUBLE_CRLF, bound_pos_) == std::string::npos) return;
  unchunkMessage(request_, buffer_.substr(bound_pos_));
}

HttpRequest HttpParser::parseHeader(const std::string& request) {
  HttpRequest request_;
  std::size_t boundary = request.find(CRLF);
  std::string request_line = request.substr(0, boundary);
  parseRequestLine(request_, request_line);

  std::string headers = request.substr(boundary + CRLF.length());
  parseHeaders(request_, headers);

  return request_;
}

void HttpParser::parseRequestLine(HttpRequest& request_,
                                  const std::string& request) {
  std::vector<std::string> request_line = split(request);
  if (request_line.size() < 3) throw std::exception();
  if (request_line[2] != "HTTP/1.1") throw std::exception();
  request_.setMethod(request_line[0]);
  request_.setUrl(request_line[1]);
}

void HttpParser::parseHeaders(HttpRequest& request_, std::string header_part) {
  std::vector<std::string> headers = splitByCRLF(header_part);

  std::vector<std::string> line;
  for (std::vector<std::string>::iterator header = headers.begin();
       header != headers.end(); ++header) {
    if (header->length() == 0) break;
    line = split(*header, ":");
    if (line.size() < 2) throw std::exception();
    request_.addheader(trim(line[0]), trim(line[1]));
  }
  request_.setHost(request_.getHeader("Host"));
}

void HttpParser::unchunkMessage(HttpRequest& request_, std::string body) {
  std::string content;
  std::size_t content_length = 0;
  std::size_t chunk_size = 0;

  std::vector<std::string> chunks = splitByCRLF(body);
  if (chunks.size() < 1) throw std::exception();

  chunk_size = hexToInt(chunks[0]);
  std::size_t idx = 1;
  while (chunk_size && idx + 1 < chunks.size()) {
    content_length += chunk_size;
    if (chunks[idx].length() != chunk_size) throw std::exception();
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

  while (true) {
    end = content.find(CRLF, start);
    if (end == std::string::npos) break;
    substrings.push_back(content.substr(start, end - start));
    start = end + CRLF.length();
  }
  substrings.push_back(content.substr(start));
  return substrings;
}
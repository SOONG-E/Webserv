#include "HttpRequest.hpp"

HttpRequest::HttpRequest() : content_length_(-1) {}

HttpRequest::HttpRequest(const HttpRequest& origin)
    : method_(origin.method_),
      uri_(origin.uri_),
      host_(origin.host_),
      content_length_(origin.content_length_),
      headers_(origin.headers_),
      body_(origin.body_) {}

HttpRequest HttpRequest::operator=(const HttpRequest& origin) {
  if (this != &origin) {
    method_ = origin.method_;
    uri_ = origin.uri_;
    host_ = origin.host_;
    content_length_ = origin.content_length_;
    headers_ = origin.headers_;
    body_ = origin.body_;
  }
  return *this;
}

HttpRequest::~HttpRequest() {}

const std::string& HttpRequest::getMethod(void) const { return method_; }

std::string& HttpRequest::getUri(void) { return uri_; }

const std::string& HttpRequest::getUri(void) const { return uri_; }

const std::string& HttpRequest::getHost(void) const { return host_; }

std::size_t HttpRequest::getContentLength(void) const {
  return content_length_;
}

std::string HttpRequest::getHeader(const std::string& key) const {
  headers_type::const_iterator iter = headers_.find(key);
  if (iter == headers_.end()) {
    return "";
  }
  return iter->second.front();
}

const std::string& HttpRequest::getBody(void) const { return body_; }

void HttpRequest::setMethod(const std::string& method) { method_ = method; }

void HttpRequest::setUri(const std::string& uri) { uri_ = uri; }

void HttpRequest::setHost(const std::string& host) { host_ = host; }

void HttpRequest::setBody(const std::string& body) { body_ = body; }

void HttpRequest::setContentLength(std::size_t content_length) {
  content_length_ = content_length;
}

void HttpRequest::addHeader(const std::string& key, const std::string& value) {
  if (0 < headers_.count(key)) {
    throw std::exception();
  }
  addHeader(key, std::vector<std::string>(1, value));
}

void HttpRequest::addHeader(const std::string& key,
                            const std::vector<std::string>& values) {
  headers_[key] = values;
}

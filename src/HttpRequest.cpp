#include "HttpRequest.hpp"

HttpRequest::HttpRequest() : content_length_(-1) {}

HttpRequest::HttpRequest(const HttpRequest &origin)
    : method_(origin.method_),
      uri_(origin.uri_),
      host_(origin.host_),
      body_(origin.body_),
      content_length_(origin.content_length_),
      headers_(origin.headers_) {}

HttpRequest HttpRequest::operator=(const HttpRequest &origin) {
  if (this != &origin) {
    method_ = origin.method_;
    uri_ = origin.uri_;
    host_ = origin.host_;
    body_ = origin.body_;
    content_length_ = origin.content_length_;
    headers_ = origin.headers_;
  }
  return *this;
}

HttpRequest::~HttpRequest() {}

void HttpRequest::setMethod(std::string method) { method_ = method; }

void HttpRequest::setUri(std::string uri) { uri_ = uri; }

void HttpRequest::setHost(std::string host) { host_ = host; }

void HttpRequest::setBody(std::string body) { body_ = body; }

void HttpRequest::setContentLength(std::size_t content_length) {
  content_length_ = content_length;
}

void HttpRequest::addheader(std::string key, std::string value) {
  if (0 < headers_.count(key)) {
    throw std::exception();
  }
  std::vector<std::string> temp;
  temp.push_back(value);
  headers_[key] = temp;
}

void HttpRequest::addheader(std::string key, std::vector<std::string> values) {
  headers_[key] = values;
}

std::string HttpRequest::getHeader(std::string name) const {
  header_type::const_iterator it = headers_.find(name);
  if (it == headers_.end()) {
    return "";
  }
  return *(it->second.begin());
}

std::string HttpRequest::getMethod() const { return (method_); }
std::string HttpRequest::getUri() const { return (uri_); }
std::string HttpRequest::getHost() const { return (host_); }
std::string HttpRequest::getBody() const { return (body_); }
std::size_t HttpRequest::getContentLength() const { return (content_length_); }

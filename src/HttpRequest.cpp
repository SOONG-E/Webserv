#include "HttpRequest.hpp"

HttpRequest::HttpRequest() : content_length_(0) {}
HttpRequest::HttpRequest(const HttpRequest &origin)
    : method_(origin.method_),
      url_(origin.url_),
      host_(origin.host_),
      body_(origin.body_),
      headers_(origin.headers_),
      content_length_(origin.content_length_) {}
HttpRequest::~HttpRequest() {}
HttpRequest HttpRequest::operator=(const HttpRequest &origin) {
  HttpRequest out(origin);

  return out;
}

void HttpRequest::setMethod(std::string method) {
  for (std::size_t i = 0; i < methods->size(); ++i) {
    if (methods[i] == method) {
      method_ = method;
      return;
    }
  }
  throw std::exception();
}

void HttpRequest::setUrl(std::string url) { url_ = url; }

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

std::string HttpRequest::getHeader(std::string name) {
  std::map<std::string, std::vector<std::string> >::iterator it;

  it = headers_.find(name);
  if (it != headers_.end()) return *(it->second.begin());
  return "";
}

bool HttpRequest::isEmpty() {
  if (method_.length() == 0) return true;
  return false;
}

std::string HttpRequest::getMethod() { return (method_); }
std::string HttpRequest::getUrl() { return (url_); }
std::string HttpRequest::getHost() { return (host_); }
std::string HttpRequest::getBody() { return (body_); }
std::size_t HttpRequest::getContentLength() { return (content_length_); }
#include "HttpRequest.hpp"

HttpRequest::HttpRequest() {}
HttpRequest::HttpRequest(const HttpRequest &origin) {}
HttpRequest::~HttpRequest() {}
HttpRequest HttpRequest::operator=(const HttpRequest &origin) { return *this; }

void HttpRequest::setMethod(std::string method) {
  for (int i = 0; i < methods->size(); ++i) {
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

void HttpRequest::addheader(std::string key, std::string value) {
  if (0 < header_.count(key)) {
    throw std::exception();
  }
  std::vector<std::string> temp;
  temp.push_back(value);
  header_[key] = temp;
}

void HttpRequest::addheader(std::string key, std::vector<std::string> values) {
  header_[key] = values;
}

std::string HttpRequest::getHeader(std::string name) {
  std::map<std::string, std::vector<std::string> >::iterator it;

  it = header_.find(name);
  if (it != header_.end()) return *(it->second.begin());
  return NULL;
}

std::string HttpRequest::getMethod() { return (method_); }
std::string HttpRequest::getUrl() { return (url_); }
std::string HttpRequest::getHost() { return (host_); }
std::string HttpRequest::getBody() { return (body_); }
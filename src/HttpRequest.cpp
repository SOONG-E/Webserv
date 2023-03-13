#include "HttpRequest.hpp"

HttpRequest::HttpRequest();
HttpRequest::HttpRequest(const HttpRequest &origin);
HttpRequest::~HttpRequest();
HttpRequest HttpRequest::operator=(const HttpRequest &origin);

void HttpRequest::setMethod(std::string method) { method_ = method; }

void HttpRequest::setUrl(std::string url) { url_ = url; }

void HttpRequest::setHost(std::string host) { host_ = host; }

void HttpRequest::addheader(std::string key, std::string value) {
  if (0 < header_.count(key)) {
    header_.find(key)->second.push_back(value);
    return;
  }
  std::vector<std::string> temp;
  temp.push_back(value);
  header_[key] = temp;
}

void addheader(std::string key, std::vector<std::string> values) {
  header_[key] = values;
}

std::string HttpRequest::getHeader(std::string name) {
  std::map<std::string, std::vector<std::string>>::iterator it;

  it = header_.find(name);
  if (it != header_.end()) return *(it->second.begin());
  return NULL;
}

std::vector<std::string> HttpRequest::getHeaders(std::string name) {
  std::map<std::string, std::vector<std::string>>::iterator it;

  it = header_.find(name);
  if (it != header_.end()) return it->second;
  return NULL;
}

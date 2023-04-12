#include "HttpRequest.hpp"

#include <stdexcept>

#include "ResponseStatus.hpp"
#include "constant.hpp"

const std::size_t HttpRequest::DEFAULT_CONTENT_LENGTH = -1;

HttpRequest::HttpRequest()
    : port_(DEFAULT_PORT), content_length_(DEFAULT_CONTENT_LENGTH) {}

HttpRequest::HttpRequest(const HttpRequest& origin)
    : method_(origin.method_),
      uri_(origin.uri_),
      host_(origin.host_),
      content_length_(origin.content_length_),
      headers_(origin.headers_),
      body_(origin.body_),
      isHeaderSet_(false),
      isCompletedRequest_(false) {}

HttpRequest HttpRequest::operator=(const HttpRequest& origin) {
  if (this != &origin) {
    method_ = origin.method_;
    uri_ = origin.uri_;
    host_ = origin.host_;
    content_length_ = origin.content_length_;
    headers_ = origin.headers_;
    body_ = origin.body_;
    isHeaderSet_ = origin.isHeaderSet_;
    isCompletedRequest_ = origin.isCompletedRequest_;
  }
  return *this;
}

HttpRequest::~HttpRequest() {}

/*==========================*/
//         Parse            //
/*==========================*/
void HttpRequest::tailRequest(std::string& message) { buffer_ += message; }
void HttpRequest::parse(void) { HttpParser::parseRequest(*this); }

/*==========================*/
//         Getter           //
/*==========================*/

const std::string& HttpRequest::getMethod(void) const { return method_; }

std::string& HttpRequest::getUri(void) { return uri_; }

const std::string& HttpRequest::getUri(void) const { return uri_; }

const std::string& HttpRequest::getQueryString(void) const {
  return query_string_;
}

const std::string& HttpRequest::getHost(void) const { return host_; }

std::size_t HttpRequest::getContentLength(void) const {
  return content_length_;
}

std::string HttpRequest::getHeader(const std::string& key) const {
  if (headers_.find(key) == headers_.end()) {
    return "";
  }
  return headers_.at(key).front();
}

std::string HttpRequest::getCookie(const std::string& name) const {
  if (cookie_.find(name) == cookie_.end()) {
    return "";
  }
  return cookie_.find(name)->second;
}

const std::string& HttpRequest::getBody(void) const { return body_; }

HttpResponse& HttpRequest::getResponse() { return response_; }

const HttpResponse& HttpRequest::getResponse() const { return response_; }

/*==========================*/
//         Setter           //
/*==========================*/

void HttpRequest::setMethod(const std::string& method) { method_ = method; }

void HttpRequest::setUri(const std::string& uri) { uri_ = uri; }

void HttpRequest::setQueryString(const std::string& query_string) {
  query_string_ = query_string;
}

void HttpRequest::setHost(const std::string& host) {
  std::size_t colon = host.find(":");
  if (colon == std::string::npos) {
    host_ = host;
    return;
  }
  host_ = host.substr(0, colon);
}

void HttpRequest::setCookie(const cookie_list_type& cookie) {
  cookie_ = cookie;
}

void HttpRequest::setBody(const std::string& body) { body_ = body; }

void HttpRequest::setStatus(const int status) { response_.setStatus(status); }

void HttpRequest::setContentLength(std::size_t content_length) {
  content_length_ = content_length;
}

void HttpRequest::addHeader(const std::string& key, const std::string& value) {
  headers_[key].push_back(value);
}

/*==========================*/
//  Check member variable   //
/*==========================*/

bool HttpRequest::hasCookie() const {
  if (getHeader("COOKIE").empty() || getCookie("Session-ID").empty()) {
    return false;
  }
  return true;
}

bool HttpRequest::isCompleted() const { return isCompletedRequest_; }

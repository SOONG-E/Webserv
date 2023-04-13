#ifndef HTTP_REQUEST_HPP_
#define HTTP_REQUEST_HPP_

#include <map>
#include <string>
#include <vector>

#include "HttpParser.hpp"
#include "HttpResponse.hpp"

class HttpRequest {
  static const std::size_t DEFAULT_CONTENT_LENGTH;

 public:
  typedef std::map<std::string, std::vector<std::string> > headers_type;

  HttpRequest();
  HttpRequest(const HttpRequest& origin);
  HttpRequest operator=(const HttpRequest& origin);
  ~HttpRequest();

  typedef std::map<std::string, std::string> cookie_list_type;

  void tailRequest(std::string& message);
  void parse(void);
  const std::string& getMethod(void) const;
  std::string& getUri(void);
  const std::string& getUri(void) const;
  const std::string& getQueryString(void) const;
  const std::string& getHost(void) const;
  std::size_t getContentLength(void) const;
  std::string getHeader(const std::string& key) const;
  std::string getCookie(const std::string& name) const;
  const std::string& getBody(void) const;

  void setMethod(const std::string& method);
  void setUri(const std::string& uri);
  void setQueryString(const std::string& query_string);
  void setHost(const std::string& host);
  void setCookie(const cookie_list_type& cookie);
  void setContentLength(std::size_t content_length);
  void addHeader(const std::string& key, const std::string& value);
  void setBody(const std::string& body);

  bool hasCookie() const;
  bool isCompleted() const;

 private:
  friend class HttpParser;
  std::string method_;
  std::string uri_;
  std::string host_;
  std::string query_string_;
  std::string port_;
  std::size_t content_length_;
  headers_type headers_;
  cookie_list_type cookie_;
  std::string body_;
  bool isHeaderSet_;
  bool isCompletedRequest_;

  std::string buffer_;
};

#endif

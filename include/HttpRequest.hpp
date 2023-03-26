#ifndef HTTP_REQUEST_HPP_
#define HTTP_REQUEST_HPP_

#include <map>
#include <string>
#include <vector>

#include "ResponseStatus.hpp"
#include "constant.hpp"
#include "exception.hpp"

class HttpRequest {
 public:
  HttpRequest();
  HttpRequest(const HttpRequest& origin);
  HttpRequest operator=(const HttpRequest& origin);
  ~HttpRequest();

  const std::string& getMethod(void) const;
  std::string& getUri(void);
  const std::string& getUri(void) const;
  const std::string& getHost(void) const;
  std::size_t getContentLength(void) const;
  std::string getHeader(const std::string& key) const;
  const std::string& getBody(void) const;

  void setMethod(const std::string& method);
  void setUri(const std::string& uri);
  void setHost(const std::string& host);
  void setContentLength(std::size_t content_length);
  void addHeader(const std::string& key, const std::string& value);
  void addHeader(const std::string& key,
                 const std::vector<std::string>& values);
  void setBody(const std::string& body);

 private:
  typedef std::map<std::string, std::vector<std::string> > headers_type;

  std::string method_;
  std::string uri_;
  std::string host_;
  std::size_t content_length_;
  headers_type headers_;
  std::string body_;
};

#endif

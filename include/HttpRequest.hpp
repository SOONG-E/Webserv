#ifndef HTTP_REQUEST_HPP_
#define HTTP_REQUEST_HPP_

#include <map>
#include <string>
#include <vector>

class HttpRequest {
 public:
  HttpRequest();
  HttpRequest(const HttpRequest& origin);
  HttpRequest operator=(const HttpRequest& origin);
  ~HttpRequest();

  const std::string& getMethod(void) const;
  const std::string& getUri(void) const;
  const std::string& getQueryString(void) const;
  const std::string& getHost(void) const;
  const std::string& getPort(void) const;
  std::size_t getContentLength(void) const;
  std::string getHeader(const std::string& key) const;
  const std::string& getBody(void) const;

  void setMethod(const std::string& method);
  void setUri(const std::string& uri);
  void setQueryString(const std::string& query_string);
  void setHost(const std::string& host);
  void setPort(const std::string& host);
  void setContentLength(std::size_t content_length);
  void addHeader(const std::string& key, const std::string& value);
  void setBody(const std::string& body);

 private:
  typedef std::map<std::string, std::vector<std::string> > headers_type;

  std::string method_;
  std::string uri_;
  std::string host_;
  std::string query_string_;
  std::string port_;
  std::size_t content_length_;
  headers_type headers_;
  std::string body_;
};

#endif

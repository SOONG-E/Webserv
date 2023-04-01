#ifndef HTTP_REQUEST_HPP_
#define HTTP_REQUEST_HPP_

#include <map>
#include <string>
#include <vector>

class HttpRequest {
  static const std::size_t DEFAULT_CONTENT_LENGTH = -1;

 public:
  HttpRequest();
  HttpRequest(const HttpRequest& origin);
  HttpRequest operator=(const HttpRequest& origin);
  ~HttpRequest();

  typedef std::map<std::string, std::string> cookie_list_type;

  const std::string& getMethod(void) const;
  const std::string& getUri(void) const;
  const std::string& getQueryString(void) const;
  const std::string& getHost(void) const;
  const std::string& getSessionId(void) const;
  std::size_t getContentLength(void) const;
  std::string getHeader(const std::string& key) const;
  std::string getCookie(const std::string& name) const;
  const std::string& getBody(void) const;

  void setMethod(const std::string& method);
  void setUri(const std::string& uri);
  void setSessionId(const std::string& session_id);
  void setQueryString(const std::string& query_string);
  void setHost(const std::string& host);
  void setCookie(const cookie_list_type& cookie);
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
  std::string session_id_;
  std::size_t content_length_;
  headers_type headers_;
  cookie_list_type cookie_;
  std::string body_;
};

#endif

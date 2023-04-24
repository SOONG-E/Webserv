#ifndef HTTP_REQUEST_HPP_
#define HTTP_REQUEST_HPP_

#include <map>
#include <string>
#include <vector>

class HttpRequest {
  static const std::size_t DEFAULT_CONTENT_LENGTH;

 public:
  typedef std::map<std::string, std::vector<std::string> > headers_type;
  typedef std::map<std::string, std::string> cookie_list_type;

  HttpRequest();
  HttpRequest(const HttpRequest& origin);
  HttpRequest operator=(const HttpRequest& origin);
  ~HttpRequest();

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
  std::string& getBuffer(void);

  void setMethod(const std::string& method);
  void setUri(const std::string& uri);
  void setQueryString(const std::string& query_string);
  void setHost(const std::string& host);
  void setCookie(const cookie_list_type& cookie);
  void setContentLength(std::size_t content_length);
  void addHeader(const std::string& key, const std::string& value);
  void setBody(const std::string& body);
  void setBuffer(const std::string& buffer);
  void setIsHeaderSet(bool is_header_set);
  void setIsCompleted(bool is_completed_request);
  void reserveBodySpace(std::size_t size);

  bool hasCookie(void) const;
  bool isHeaderSet(void) const;
  bool isCompleted(void) const;

  void clear(void);

 private:
  std::string method_;
  std::string uri_;
  std::string host_;
  std::string query_string_;
  std::string port_;
  std::size_t content_length_;
  headers_type headers_;
  cookie_list_type cookie_;
  std::string body_;
  bool is_header_set_;
  bool is_completed_request_;

  std::string buffer_;
};

#endif

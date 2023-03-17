#ifndef HTTP_REQUEST_HPP_
#define HTTP_REQUEST_HPP_

#include <map>
#include <string>
#include <vector>

#include "constant.hpp"

class HttpRequest {
 public:
  HttpRequest();
  HttpRequest(const HttpRequest &origin);
  ~HttpRequest();
  HttpRequest operator=(const HttpRequest &origin);

  void setMethod(std::string method);
  void setUrl(std::string url);
  void setHost(std::string host);
  void setBody(std::string body);
  void setContentLength(std::size_t content_length);
  void addheader(std::string key, std::string value);
  void addheader(std::string key, std::vector<std::string> values);
  std::string getHeader(std::string name);
  std::string getMethod();
  std::string getUrl();
  std::string getHost();
  std::string getBody();
  std::size_t getContentLength();

 private:
  std::string method_;
  std::string url_;
  std::string host_;
  std::string body_;
  std::size_t content_length_;
  std::map<std::string, std::vector<std::string> > header_;
};

#endif
#ifndef HTTP_REQUEST_HPP_
#define HTTP_REQUEST_HPP_

#include <map>
#include <string>
#include <vector>

#include "Method.hpp"

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
  void addheader(std::string key, std::string value);
  void addheader(std::string key, std::vector<std::string> values);
  std::string getHeader(std::string name);

 private:
  std::string method_;
  std::string url_;
  std::string host_;
  std::string body_;
  std::map<std::string, std::vector<std::string>> header_;
};

#endif
#ifndef HTTP_REQUEST_HPP_
#define HTTP_REQUEST_HPP_

#include <map>
#include <string>
#include <vector>

#include "exception.hpp"
#include "ResponseStatus.hpp"
#include "constant.hpp"

class HttpRequest {
 public:
  HttpRequest();
  HttpRequest(const HttpRequest &origin);
  HttpRequest operator=(const HttpRequest &origin);
  ~HttpRequest();

  void setMethod(std::string method);
  void setUri(std::string uri);
  void setHost(std::string host);
  void setBody(std::string body);
  void setContentLength(std::size_t content_length);
  void addheader(std::string key, std::string value);
  void addheader(std::string key, std::vector<std::string> values);
  std::string getHeader(std::string name) const;
  std::string getMethod() const;
  std::string getUri() const;
  std::string getHost() const;
  std::string getBody() const;
  std::size_t getContentLength() const;
  class NotImplemented : public ExceptionTemplate {
   public:
    NotImplemented() : ExceptionTemplate(ResponseStatus::REASONS[C413]) {}
  };

 private:
  typedef std::map<std::string, std::vector<std::string> > header_type;

  std::string method_;
  std::string uri_;
  std::string host_;
  std::string body_;
  std::size_t content_length_;
  header_type headers_;
};

#endif
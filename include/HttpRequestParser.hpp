#ifndef HTTP_REQUEST_PARSER_HPP_
#define HTTP_REQUEST_PARSER_HPP_

#include "HttpRequest.hpp"

class HttpRequestParser {
 public:
  static HttpRequest parse(const std::string request);

 private:
  void checkRequestLine(const std::vector<std::string>& request_line);
  void handlePost(HttpRequest& http_request, std::string request);
};

#endif

#ifndef HTTP_REQUEST_PARSER_HPP_
#define HTTP_REQUEST_PARSER_HPP_

#include "HttpRequest.hpp"
#include "utility.hpp"

class HttpParser {
 public:
  static HttpRequest parse(const std::string request);
  static std::string crlf;

 private:
  static std::vector<std::string> splitByCRLF(const std::string &content);
  static void parseRequestLine(HttpRequest &http_request,
                               const std::string &request);
  static void parseHeader(HttpRequest &http_request, std::string headers);
  static void handlePost(HttpRequest &http_request, std::string request);
  static void unchunkMessage(HttpRequest &http_request, std::string body);
};

#endif

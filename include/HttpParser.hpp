#ifndef HTTP_PARSER_HPP_
#define HTTP_PARSER_HPP_

#include <string>
#include <vector>

#include "HttpRequest.hpp"

class HttpRequest;

class HttpParser {
  static const std::size_t HEADER_MAX_SIZE;

 public:
  static void parseRequest(HttpRequest& request);

 private:
  static void parseHeader(HttpRequest& request);
  static void separateHeader(HttpRequest& request,
                             const std::string& header_part);
  static void reserveBodySpace(HttpRequest& request);
  static void parseRequestLine(HttpRequest& request,
                               const std::string& request_line);
  static void parseHeaderFields(HttpRequest& request,
                                const std::string& header_part);
  static void parseCookie(HttpRequest& request);
  static void parseQueryString(HttpRequest& request);
  static void parsebBody(HttpRequest& request);
  static void unchunkMessage(HttpRequest& request);
  static std::vector<std::string> splitByCRLF(const std::string& content);

 private:
  HttpParser(){};
  ~HttpParser(){};
};

#endif

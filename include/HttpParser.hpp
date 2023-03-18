#ifndef HTTP_PARSER_HPP_
#define HTTP_PARSER_HPP_

#include "HttpRequest.hpp"
#include "constant.hpp"
#include "utility.hpp"

class HttpParser {
 public:
  explicit HttpParser(const std::string& socket_buffer = "");
  HttpParser(const HttpParser& origin);
  HttpParser& operator=(const HttpParser& origin);
  ~HttpParser();

  std::size_t getContentLength(void);
  const HttpRequest& getRequest(void) const;
  const std::string& getBuffer(void) const;

  void appendBuffer(const std::string& socket_buffer);

 private:
  void setHeader(void);
  void handlePost(void);

  static HttpRequest parseHeader(const std::string& request);
  static void parseRequestLine(HttpRequest& http_request,
                               const std::string& request);
  static void parseHeaders(HttpRequest& http_request, std::string headers);
  static void unchunkMessage(HttpRequest& http_request, std::string body);
  static std::vector<std::string> splitByCRLF(const std::string& content);

  HttpRequest request_;
  std::string buffer_;
  std::size_t bound_pos_;
};

#endif
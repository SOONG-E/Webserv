#ifndef HTTP_PARSER_HPP_
#define HTTP_PARSER_HPP_

#include "HttpRequest.hpp"
#include "exception.hpp"
#include "utility.hpp"

class HttpParser {
 public:
  explicit HttpParser(const std::string& socket_buffer = "");
  HttpParser(const HttpParser& origin);
  HttpParser& operator=(const HttpParser& origin);
  ~HttpParser();

  std::size_t getContentLength(void);
  const HttpRequest& getRequestObj(void) const;
  const std::string& getBuffer(void) const;

  void appendRequest(const std::string& socket_buffer);
  bool isCompleted(void) const;
  void clear(void);

 private:
  static HttpRequest parseHeader(const std::string& request);
  static void parseRequestLine(HttpRequest& http_request,
                               const std::string& request);
  static void parseHeaders(HttpRequest& http_request, std::string headers);
  static void unchunkMessage(HttpRequest& http_request, std::string body);
  static std::vector<std::string> splitByCRLF(const std::string& content);

  bool isHeaderSet(void) const;
  void setHeader(void);
  void handlePost(void);

  HttpRequest request_;
  std::string buffer_;
  std::size_t bound_pos_;
};

#endif

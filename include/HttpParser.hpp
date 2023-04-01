#ifndef HTTP_PARSER_HPP_
#define HTTP_PARSER_HPP_

#include <string>
#include <vector>

#include "HttpRequest.hpp"

class HttpParser {
  static const std::size_t HEADER_MAX_SIZE = 8192;

 public:
  explicit HttpParser(const std::string& socket_buffer = "");
  HttpParser(const HttpParser& origin);
  HttpParser& operator=(const HttpParser& origin);
  ~HttpParser();

  std::size_t getContentLength(void);
  HttpRequest& getRequestObj(void);
  const HttpRequest& getRequestObj(void) const;
  const std::string& getBuffer(void) const;

  void appendRequest(const std::string& socket_buffer);
  bool isCompleted(void) const;
  void clear(void);

 private:
  bool isHeaderSet(void) const;
  void setHeader(void);
  void handlePost(void);
  void parseHeader(const std::string& header_part);
  void parseCookie(void);
  void parseQueryString(void);
  void parseRequestLine(const std::string& request_line);
  void parseHeaderFields(const std::string& header_part);
  void unchunkMessage(const std::string& body_part);
  std::vector<std::string> splitByCRLF(const std::string& content);

  HttpRequest request_;
  std::string buffer_;
  std::size_t bound_pos_;
};

#endif

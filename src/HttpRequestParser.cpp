#include "HttpRequestParser.hpp"

#include <exception>
#include <iostream>

std::vector<std::string> split(const std::string& content,
                               const std::string& separators = " \t\n\v\f\r") {
  std::vector<std::string> substrings;
  std::size_t start = 0;
  std::size_t end;
  while (true) {
    end = content.find_first_of(separators, start);
    if (end == std::string::npos) break;
    substrings.push_back(content.substr(start, end - start));
    start = end + 1;
  }
  substrings.push_back(content.substr(start));
  return substrings;
}

std::vector<std::string> splitByCRLF(const std::string& content) {
  std::vector<std::string> substrings;
}

void HttpRequestParser::checkRequestLine(
    const std::vector<std::string>& request_line) {
  if (request_line.size() < 3) throw std::exception();
  if (request_line[2] != "HTTP/1.1") throw std::exception();
}

void HttpRequestParser::handlePost(HttpRequest& http_request,
                                   std::string request) {
  std::string crlf = std::string("\r\n\r\n");
  size_t bodyIdx = request.find(crlf);

  if (bodyIdx == std::string::npos) throw std::exception();
  std::string body = request.substr(bodyIdx + crlf.length(), request.length());
  http_request.setBody(body);
}

HttpRequest HttpRequestParser::parse(const std::string request) {
  HttpRequest http_request;

  std::string request_line = request.substr(0, request.find("\r\n"));
  std::vector<std::string> requests = split(request_line);
  std::vector<std::string> header_part = splitByCRLF(headerPart);
  checkRequestLine(requests);
  http_request.setMethod(requests[0]);
  http_request.setUrl(requests[1]);

  headers.erase(headers.begin());
  std::vector<std::string> line;
  for (std::vector<std::string>::iterator header = headers.begin();
       header != headers.end(); ++header) {
    line = split(*header, ":");
    if (line.size() < 2) throw std::exception();
    http_request.addheader(line[0], line[1]);
    line.erase(line.begin());
  }
  // http_request.setHost(http_request.getHeader("host"));
  handlePost(http_request, request);

  return http_request;
}
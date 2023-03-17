#include "HttpParser.hpp"

#include <exception>
#include <iostream>

std::string HttpParser::crlf = std::string("\r\n");

std::string trim(std::string str) {
  std::size_t start = str.find_first_not_of(" \t\n\v\f\r");
  std::size_t end = str.find_last_not_of(" \t\n\v\f\r");

  return (str.substr(start, end + 1));
}

std::vector<std::string> HttpParser::splitByCRLF(const std::string& content) {
  std::vector<std::string> substrings;
  std::size_t start = 0;
  std::size_t end;

  while (true) {
    end = content.find(HttpParser::crlf, start);
    if (end == std::string::npos) break;
    substrings.push_back(content.substr(start, end - start));
    start = end + HttpParser::crlf.length();
  }
  substrings.push_back(content.substr(start));
  return substrings;
}

void HttpParser::parseRequestLine(HttpRequest& http_request,
                                  const std::string& request) {
  std::vector<std::string> request_line = split(request);
  if (request_line.size() < 3) throw std::exception();
  if (request_line[2] != "HTTP/1.1") throw std::exception();
  http_request.setMethod(request_line[0]);
  http_request.setUrl(request_line[1]);
}

void HttpParser::parseHeader(HttpRequest& http_request,
                             std::string header_part) {
  std::vector<std::string> headers = splitByCRLF(header_part);

  std::vector<std::string> line;
  for (std::vector<std::string>::iterator header = headers.begin();
       header != headers.end(); ++header) {
    if ((*header).length() == 0) break;
    line = split(*header, ":");
    if (line.size() < 2) throw std::exception();
    http_request.addheader(trim(line[0]), trim(line[1]));
  }
  http_request.setHost(http_request.getHeader("Host"));
}

void HttpParser::unchunkMessage(HttpRequest& http_request, std::string body) {
  std::string content;
  std::size_t content_length = 0;
  std::size_t chunk_size = 0;

  std::vector<std::string> chunks = splitByCRLF(body);
  if (chunks.size() < 1) throw std::exception();

  chunk_size = hexToInt(chunks[0]);
  std::size_t idx = 1;
  while (chunk_size && idx + 1 < chunks.size()) {
    content_length += chunk_size;
    content += chunks[idx++];
    chunk_size = hexToInt(chunks[idx++]);
  }
  http_request.setContentLength(content_length);
  http_request.setBody(content);
}

void HttpParser::handlePost(HttpRequest& http_request, std::string request) {
  std::string crlf = std::string("\r\n\r\n");
  std::size_t boundary = request.find(crlf);
  if (boundary == std::string::npos) throw std::exception();

  std::string header_part = request.substr(0, boundary);
  std::string body = request.substr(boundary + crlf.length());

  parseHeader(http_request, header_part);
  std::string content_length = http_request.getHeader("Content-Length");
  if (content_length != "") {
    http_request.setContentLength(::stoi(content_length));
    http_request.setBody(body);
    return;
  }
  if (http_request.getHeader("Transfer-Encoding") == "") throw std::exception();
  if (http_request.getHeader("Transfer-Encoding") == "chunked")
    unchunkMessage(http_request, body);
}

HttpRequest HttpParser::parse(const std::string request) {
  HttpRequest http_request;
  std::size_t boundary = request.find(crlf);
  std::string request_line = request.substr(0, boundary);
  parseRequestLine(http_request, request_line);

  std::string headers = request.substr(boundary + crlf.length());
  if (http_request.getMethod() == "POST") {
    handlePost(http_request, headers);
    return http_request;
  }
  parseHeader(http_request, headers);
  return http_request;
}

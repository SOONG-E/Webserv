#include "HttpParser.hpp"

#include <algorithm>
#include <stdexcept>

#include "Error.hpp"
#include "ResponseStatus.hpp"
#include "constant.hpp"
#include "exception.hpp"
#include "utility.hpp"

const std::size_t HttpParser::HEADER_MAX_SIZE = 8192;

/*===============================================================*/
// parse Http request and create HttpRequest instance
//
// 1. parseHeader if isHeader is True
//  1-1. separate request using DOUBLE_CRLF
//  1-2. separate header part into request_line, headers
//    1-2-a. validate the information
//    1-2-b. throw exception once detecting any error
//           (set error, status after catch)
//  1-3. parse and validate the body only if method is POST
//    1-3-a. if Transfer-Encoding is "chunked" and
//           there isn't a Content-Length header
//           then unchunk messages and calculate content length
/*===============================================================*/

void HttpParser::parseRequest(HttpRequest& request) {
  if (request.isHeaderSet() == false) {
    parseHeader(request);
  }
  if (request.isHeaderSet() == true && request.isCompleted() == false) {
    parsebBody(request);
  }
}

void HttpParser::parseHeader(HttpRequest& request) {
  std::string& buffer = request.getBuffer();
  std::size_t bound_pos = buffer.find(DOUBLE_CRLF);
  if (buffer.size() <= HEADER_MAX_SIZE && bound_pos == std::string::npos) {
    return;
  }
  if (HEADER_MAX_SIZE < bound_pos) {
    throw ResponseException(C413);
  }
  request.setIsHeaderSet(true);
  separateHeader(request, buffer.substr(0, bound_pos));
  reserveBodySpace(request);
  request.setBuffer(buffer.substr(bound_pos + DOUBLE_CRLF.size()));
}

void HttpParser::separateHeader(HttpRequest& request,
                                const std::string& header_part) {
  std::size_t boundary = header_part.find(CRLF);
  std::string request_line = header_part.substr(0, boundary);
  parseRequestLine(request, request_line);
  std::string headers = header_part.substr(boundary + CRLF.length());
  parseHeaderFields(request, headers);
}

void HttpParser::reserveBodySpace(HttpRequest& request) {
  std::string content_length = request.getHeader("CONTENT-LENGTH");
  if (content_length.empty() == true) {
    request.setContentLength(0);
    return;
  }
  std::size_t length = ::stoi(content_length);
  request.reserveBodySpace(length);
}

void HttpParser::parseRequestLine(HttpRequest& request,
                                  const std::string& request_line) {
  std::vector<std::string> request_words = split(request_line);
  if (request_words.size() < 3 || request_words[2].length() < 6 ||
      request_words[2].substr(0, 4) != "HTTP") {
    throw ResponseException(C400);
  }
  if (request_words[2].substr(0, 5) != "HTTP/" ||
      request_words[2].substr(5) != "1.1") {
    throw ResponseException(C505);
  }
  request.setMethod(request_words[0]);
  request.setUri(request_words[1]);
  parseQueryString(request);
}

void HttpParser::parseHeaderFields(HttpRequest& request,
                                   const std::string& header_part) {
  std::vector<std::string> headers = splitByCRLF(header_part);
  std::vector<std::string> line;
  for (std::vector<std::string>::const_iterator header = headers.begin();
       header != headers.end(); ++header) {
    if (header->length() == 0) break;
    line = split(*header, ":");
    if (line.size() < 2) {
      throw ResponseException(C400);
    }
    std::transform(line[0].begin(), line[0].end(), line[0].begin(), ::toupper);
    request.addHeader(trim(line[0]), trim(line[1]));
  }

  request.setHost(request.getHeader("HOST"));
  if (request.getHost().empty()) {
    throw ResponseException(C400);
  }
  if (request.getHeader("CONTENT-TYPE").empty()) {
    request.addHeader("CONTENT-TYPE", "application/octet-stream");
  }
  if (!request.getHeader("COOKIE").empty()) {
    parseCookie(request);
  }
}

void HttpParser::parseCookie(HttpRequest& request) {
  HttpRequest::cookie_list_type request_cookie_list;

  std::string cookies = request.getHeader("COOKIE");
  std::vector<std::string> cookie_list = split(cookies, ";");
  std::vector<std::string> values;

  for (std::vector<std::string>::iterator it = cookie_list.begin();
       it != cookie_list.end(); ++it) {
    values = split(*it, "=");
    if (values.size() != 2) {
      throw ResponseException(C400);
    }
    request_cookie_list[trim(values[0])] = trim(values[1]);
  }
  request.setCookie(request_cookie_list);
}

void HttpParser::parseQueryString(HttpRequest& request) {
  const std::string& uri = request.getUri();
  std::size_t query_boundary = uri.find("?");
  if (uri == "?" || query_boundary == std::string::npos) {
    return;
  }
  std::string query_string = uri.substr(query_boundary + 1);
  request.setUri(uri.substr(0, query_boundary));
  if (request.getMethod() == "GET" || request.getMethod() == "HEAD") {
    request.setQueryString(query_string);
  }
}

/*==========================*/
//    handle post method    //
/*==========================*/

void HttpParser::parsebBody(HttpRequest& request) {
  if (request.getMethod() != METHODS[POST]) {
    request.setIsCompleted(true);
    request.setBuffer("");
    return;
  }
  std::string buffer = request.getBuffer();
  if (!request.getHeader("CONTENT-LENGTH").empty()) {
    if (request.getContentLength() == static_cast<std::size_t>(-1)) {
      try {
        request.setContentLength(::stoi(request.getHeader("CONTENT-LENGTH")));
      } catch (std::invalid_argument& e) {
        throw ResponseException(C400);
      }
    }
    std::size_t content_length = request.getContentLength();
    if (content_length < buffer.size()) {
      throw ResponseException(C413);
    }
    if (content_length == buffer.size()) {
      request.setBody(buffer);
      request.setIsCompleted(true);
    }
    return;
  }
  if (request.getHeader("TRANSFER-ENCODING") != "chunked") {
    throw ResponseException(C411);
  }
  if (buffer.find(DOUBLE_CRLF) == std::string::npos) return;
  unchunkMessage(request);
}

void HttpParser::unchunkMessage(HttpRequest& request) {
  std::string content;
  std::size_t content_length = 0;
  std::size_t chunk_size = 0;
  std::vector<std::string> chunks = splitByCRLF(request.getBuffer());
  if (chunks.size() < 1) {
    throw ResponseException(C400);
  }
  chunk_size = hexToInt(chunks[0]);
  std::size_t idx = 1;
  while (chunk_size && idx + 1 < chunks.size()) {
    content_length += chunk_size;
    if (chunks[idx].length() != chunk_size) throw ResponseException(C400);
    content += chunks[idx++];
    chunk_size = hexToInt(chunks[idx++]);
  }
  request.setContentLength(content_length);
  request.setBody(content);
  request.setIsCompleted(true);
}

/*==========================*/
//           util           //
/*==========================*/

std::vector<std::string> HttpParser::splitByCRLF(const std::string& content) {
  std::vector<std::string> substrings;
  std::size_t start = 0;
  std::size_t end;
  while (start != content.size()) {
    end = content.find(CRLF, start);
    if (end == std::string::npos) {
      substrings.push_back(content.substr(start));
      break;
    }
    substrings.push_back(content.substr(start, end - start));
    start = end + CRLF.length();
  }
  return substrings;
}

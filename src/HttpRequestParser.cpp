#include "HttpRequestParser.hpp"

std::vector<std::string> split(const std::string& content,
                               const std::string& separators = " \t\n\v\f\r") {
  std::vector<std::string> substrings;
  std::size_t start = 0;
  std::size_t end;
  while (true) {
    end = content.find_first_of(separators);
    if (end == std::string::npos) break;
    substrings.push_back(content.substr(start, end - start));
    start = end + 1;
  }
  substrings.push(content.substr(start));
  return substrings;
}

HttpRequest HttpRequestParser::parse(const std::string request) {
  HttpRequest http_request;

  std::vector<std::string> headers = split(request, "\n");
  std::vector<std::string> request_line = split(*(headers.begin()));
  if (request_line.size < 3) throw exception;
  http_request.setMethod(request_line[0]);
  http_request.setUrl(request_line[1]);

  headers.erase(headers.begin());
  std::vector<std::string> temp;
  std::string key;
  for (std::vector<std::string>::iterator header = headers.begin();
       header != headers.end(); ++header) {
    temp = split(*header, ":");
    if (temp.size < 2) throw exception;
    key = temp[0];
    temp.erase(temp.begin);
    if (temp) temp = split(temp, ",");
  }

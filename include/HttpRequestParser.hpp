#ifndef HTTP_REQUEST_PARSER_HPP_
#define HTTP_REQUEST_PARSER_HPP_

#include "HttpRequest.hpp"

class HttpRequestParser {
 public:
	static HttpRequest parse(const std::string request);
 private:
};

#endif

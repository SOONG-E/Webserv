#ifndef RESPONSE_HPP_
#define RESPONSE_HPP_

#include <map>
#include <string>

struct Response {
  std::map<std::string, std::string> headers;
  std::string body;
};

#endif
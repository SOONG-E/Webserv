#ifndef RESPONSE_STATUS_HPP_
#define RESPONSE_STATUS_HPP_

#include <map>
#include <string>

enum StatusIndex {
  C200,
  C201,
  C204,
  C303,
  C400,
  C403,
  C404,
  C405,
  C411,
  C413,
  C500,
  C501,
  C504,
  C505,
};

struct ResponseStatus {
  static const std::string CODES[];
  static const std::string REASONS[];
};

#endif

#ifndef RESPONSE_EXCEPTION_HPP_
#define RESPONSE_EXCEPTION_HPP_

#include "exception/ExceptionTemplate.hpp"

struct ResponseException : ExceptionTemplate {
  explicit ResponseException(const int idx);

  int index;
};

#endif

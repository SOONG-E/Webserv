#ifndef RESPONSE_EXCEPTION_HPP_
#define RESPONSE_EXCEPTION_HPP_

#include <stdexcept>

#include "ResponseStatus.hpp"

struct ResponseException : public std::runtime_error {
  explicit ResponseException(const int index)
      : std::runtime_error(ResponseStatus::REASONS[index]), status(index) {}

  int status;
};

#endif

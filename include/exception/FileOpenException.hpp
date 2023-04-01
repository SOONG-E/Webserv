#ifndef FILE_OPEN_EXCEPTION_HPP_
#define FILE_OPEN_EXCEPTION_HPP_

#include "Error.hpp"
#include "ExceptionTemplate.hpp"

class FileOpenException : public ExceptionTemplate {
 public:
  explicit FileOpenException(const std::string& info = Error::INFO[EOPEN])
      : ExceptionTemplate(info) {}
};

#endif

#ifndef FILE_OPEN_EXCEPTION_HPP_
#define FILE_OPEN_EXCEPTION_HPP_

#include "ExceptionTemplate.hpp"
#include "constant.hpp"

class FileOpenException : public ExceptionTemplate {
 public:
  explicit FileOpenException(const std::string& err_info = ERRORS[OPEN])
      : ExceptionTemplate(err_info) {}
};

#endif

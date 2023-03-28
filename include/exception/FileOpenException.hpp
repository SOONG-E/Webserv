#ifndef FILE_OPEN_EXCEPTION_HPP_
#define FILE_OPEN_EXCEPTION_HPP_

#include "ExceptionTemplate.hpp"
#include "constant.hpp"

class FileOpenException : public ExceptionTemplate {
 public:
  FileOpenException() : ExceptionTemplate(ERRORS[OPEN]) {}
  explicit FileOpenException(const std::string& err_info)
      : ExceptionTemplate(err_info) {}
};

#endif

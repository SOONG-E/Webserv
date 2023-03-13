#ifndef EXCEPTION_TEMPLATE_HPP_
#define EXCEPTION_TEMPLATE_HPP_

#include <exception>
#include <string>

#include "constant.hpp"

class ExceptionTemplate : public exception {
 public:
  explicit ExceptionTemplate(const std::string& err_info)
      : err_msg_(kErrors[kPrefix] + err_info) {}

 protected:
  std::string err_msg_;
  virtual const char* what() const throw() { return err_msg_.c_str(); }
};

#endif

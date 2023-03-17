#ifndef EXCEPTION_TEMPLATE_HPP_
#define EXCEPTION_TEMPLATE_HPP_

#include <stdexcept>
#include <string>

#include "constant.hpp"

class ExceptionTemplate : public std::runtime_error {
 public:
  explicit ExceptionTemplate(const std::string& err_info)
      : std::runtime_error(kErrors[kPrefix] + err_info) {}
};

#endif

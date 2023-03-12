#ifndef CONFIG_EXCEPTION_HPP_
#define CONFIG_EXCEPTION_HPP_

#include "ExceptionTemplate.hpp"

class ConfigException : public ExceptionTemplate {
 public:
  ConfigException(const std::string& err_msg) : ExceptionTemplate(err_msg) {}
};

#endif

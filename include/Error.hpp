#ifndef ERROR_HPP_
#define ERROR_HPP_

#include <string>

enum ErrorIndex {
  EPREFIX,
  EARG,
  EOPEN,
  ETOKEN,
};

struct Error {
  static const std::string PREFIX;
  static const std::string INFO[];

  static void log(const std::string& err_info, int exit_status = 0);
};

#endif

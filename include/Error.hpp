#ifndef ERROR_HPP_
#define ERROR_HPP_

#include <string>

enum ErrorIndex { EARG, ECGI, EOPEN, ERECV, ESEND, ETOKEN, ESYSTEM };

struct Error {
  static const std::string DELIM;
  static const std::string PREFIX;
  static const std::string INFO[];

  static void log(const std::string& err_info, const std::string& arg = "",
                  int exit_status = 0);
};

#endif

#include "Error.hpp"

#include <cstdlib>
#include <iostream>

const std::string Error::PREFIX = "Error: ";
const std::string Error::INFO[] = {"Invalid argument", "File open failed",
                                   "Unexpected token"};

void Error::log(const std::string& err_info, int exit_status) {
  std::cerr << PREFIX << err_info << std::endl;
  if (exit_status) {
    exit(exit_status);
  }
}

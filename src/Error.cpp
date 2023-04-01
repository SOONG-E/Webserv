#include "Error.hpp"

#include <cstdlib>
#include <iostream>

const std::string Error::DELIM = ": ";
const std::string Error::PREFIX = "Error" + DELIM;
const std::string Error::INFO[] = {
    "Invalid argument",  // EARG
    "Cgi I/O failed",    // ECGI
    "File open failed",  // EOPEN
    "Receive failed",    // ERECV
    "Send failed",       // ESEND
    "Unexpected token",  // ETOKEN
};

void Error::log(const std::string& info, const std::string& arg,
                int exit_status) {
  std::cerr << PREFIX << info;
  if (!arg.empty()) {
    std::cerr << DELIM << arg;
  }
  std::cerr << std::endl;
  if (exit_status) {
    std::exit(exit_status);
  }
}

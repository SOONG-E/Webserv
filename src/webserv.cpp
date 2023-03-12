#include "webserv.hpp"

#include <fstream>
#include <iostream>

#include "constant.hpp"

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cout << kErrors[kPrefix] << kErrors[kArg] << '\n';
    return 1;
  }
  try {
    ConfigParser.parse(argv[1]);
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
    return 1;
  }

  return 0;
}

// #include "webserv.hpp"

#include <iostream>

#include "ConfigParser.hpp"
#include "constant.hpp"

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cout << kErrors[kPrefix] << kErrors[kArg] << '\n';
    return 1;
  }
  try {
    ConfigParser conf(argv[1]);
    conf.parse();
    conf.print();
  } catch (const std::exception& e) {
    std::cerr << e.what() << "\n";
    return 1;
  }
  return 0;
}

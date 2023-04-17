#include "bootServer.hpp"
#include "constant.hpp"

int main(int argc, char* argv[]) {
  ServerManager manager = setServer(argc, argv);
  runServer(manager);

  return EXIT_SUCCESS;
}

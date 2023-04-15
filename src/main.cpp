#include "bootServer.hpp"
#include "constant.hpp"

void runServer(const ServerManager& manager) {
  while (true) {
  }
}

int main(int argc, char* argv[]) {
  checkArgs(argc);
  printLogo();

  const Config config = createConfig(argc, argv);
  const ServerManager manager(config);
  
  setServer(manager);

  runServer(manager);

  return EXIT_SUCCESS;
}

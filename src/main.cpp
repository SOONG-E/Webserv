#include "bootServer.hpp"
#include "constant.hpp"

void runServer(ServerManager& manager) {
  while (1) {
    if (manager.select() > 0) {
      manager.respondToClients();
      manager.acceptConnections();
    }
    manager.handleTimeout();
  }
}

int main(int argc, char* argv[]) {
  checkArgs(argc);
  printLogo();

  const Config config = createConfig(argc, argv);
  ServerManager manager = setServer(config);

  runServer(manager);

  return EXIT_SUCCESS;
}

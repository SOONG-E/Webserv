#include <iostream>

#include "ConfigParser.hpp"
#include "ServerHandler.hpp"
#include "constant.hpp"

int main(int argc, char* argv[]) {
  if (argc > 2) {
    std::cerr << ERRORS[PREFIX] << ERRORS[ARG] << std::endl;
    return EXIT_FAILURE;
  }

  const char* filename = (argc == 2) ? argv[1] : "conf/default.conf";

  try {
    ConfigParser conf(filename);
    const Config& config = conf.parse();

    ServerHandler handler;
    handler.configureServer(config);
    handler.createServers();

    while (1) {
      handler.acceptConnections();
      handler.respondToClients();
    }

  } catch (const std::exception& e) {
    std::cerr << e.what() << "\n";
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

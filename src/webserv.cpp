#include <iostream>

#include "ConfigParser.hpp"
#include "ServerHandler.hpp"
#include "constant.hpp"

// temp
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

int main(int argc, char* argv[]) {
  if (argc > 2) {
    std::cerr << ERRORS[PREFIX] << ERRORS[ARG] << std::endl;
    return EXIT_FAILURE;
  }

  const std::string& filename = (argc == 2) ? argv[1] : DEFAULT_PATH;

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

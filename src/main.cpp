#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

#include "Config.hpp"
#include "ConfigParser.hpp"
#include "ServerHandler.hpp"
#include "constant.hpp"

int main(int argc, char* argv[]) {
  if (argc > 2) {
    std::cerr << ERRORS[PREFIX] << ERRORS[ARG] << std::endl;
    return EXIT_FAILURE;
  }

  const std::string& filename = (argc == 2) ? argv[1] : DEFAULT_PATH;

  try {
    ConfigParser config_parser(filename);
    const Config& config = config_parser.parse();

    ServerHandler handler;
    handler.registerSignalHandlers();
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

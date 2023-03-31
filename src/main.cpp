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

  ServerHandler handler;
  ConfigParser config_parser(filename);
  const Config& config = config_parser.parse();

  handler.registerSignalHandlers();
  handler.configureServer(config);
  handler.createServers();

  while (1) {
    handler.acceptConnections();
    handler.respondToClients();
  }
  return EXIT_SUCCESS;
}

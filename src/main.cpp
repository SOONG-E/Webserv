#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

#include "Config.hpp"
#include "ConfigParser.hpp"
#include "Error.hpp"
#include "ServerHandler.hpp"
#include "constant.hpp"

int main(int argc, char* argv[]) {
  if (argc > 2) {
    Error::log(Error::INFO[EARG], "", EXIT_FAILURE);
  }

  const std::string& filename = (argc == 2) ? argv[1] : DEFAULT_PATH;

  ConfigParser config_parser(filename);
  const Config& config = config_parser.parse();

  ServerHandler handler;
  handler.registerSignalHandlers();
  handler.configureServer(config);
  handler.createServers();

  while (1) {
    if (handler.select() > 0) {
      handler.acceptConnections();
      handler.respondToClients();
      std::cout << "asdfasdf" << std::endl;
    }
    handler.handleTimeout();
  }
  return EXIT_SUCCESS;
}

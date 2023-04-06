#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

#include "Config.hpp"
#include "ConfigParser.hpp"
#include "Error.hpp"
#include "ServerManager.hpp"
#include "constant.hpp"
#include "utility.hpp"

int main(int argc, char* argv[]) {
  if (argc > 2) {
    Error::log(Error::INFO[EARG], "", EXIT_FAILURE);
  }
  std::cout << readFile("Webserv.art");

  const std::string& filename = (argc == 2) ? argv[1] : DEFAULT_PATH;
  ConfigParser config_parser(filename);
  const Config& config = config_parser.parse();

  ServerManager manager;
  manager.registerSignalHandlers();
  manager.configureServer(config);
  manager.createServers();

  while (1) {
    if (manager.select() > 0) {
      manager.respondToClients();
      manager.acceptConnections();
    }
    manager.handleTimeout();
  }
  return EXIT_SUCCESS;
}

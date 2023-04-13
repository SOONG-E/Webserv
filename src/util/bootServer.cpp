#include "bootServer.hpp"

void checkArgs(int argc) {
  if (argc > 2) {
    Error::log(Error::INFO[EARG], "", EXIT_FAILURE);
  }
}

void printLogo() { std::cout << readFile("Webserv.art"); }

Config createConfig(int argc, char** argv) {
  const std::string& filename = (argc == 2) ? argv[1] : DEFAULT_PATH;
  ConfigParser config_parser(filename);
  Config config = config_parser.parse();

  return config;
}

ServerManager setServer(const Config& config) {
  ServerManager manager;
  manager.registerSignalHandlers();
  manager.configureServer(config);
  manager.createServers();
  return manager;
}
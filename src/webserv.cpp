// #include "webserv.hpp"

#include <iostream>

#include "ConfigParser.hpp"
#include "ServerHandler.hpp"
#include "constant.hpp"

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << kErrors[kPrefix] << kErrors[kArg] << '\n';
    return EXIT_FAILURE;
  }
  try {
    ConfigParser conf(argv[1]);
    ServerHandler handler;

    const Config& config = conf.parse();
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

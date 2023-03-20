// #include "webserv.hpp"

#include <iostream>

#include "ConfigParser.hpp"
#include "ServerHandler.hpp"
#include "constant.hpp"

int main(int argc, char* argv[]) {
  if (argc > 2) {
    std::cerr << ERRORS[PREFIX] << ERRORS[ARG] << std::endl;
    return 1;
  }
  const char* filename = (argc == 2) ? argv[1] : "conf/default.conf";
  try {
    ConfigParser conf(filename);
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
    return 1;
  }
  return 0;
}

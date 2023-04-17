#include "bootServer.hpp"

static void checkArgs(int argc) {
  if (argc > 2) {
    Error::log(Error::INFO[EARG], "", EXIT_FAILURE);
  }
}

static void printLogo() { std::cout << readFile("Webserv.art"); }

static void registerSignalHandlers() {
  signal(SIGPIPE, SIG_IGN);
  signal(SIGCHLD, SIG_IGN);
}

static Config createConfig(int argc, char** argv) {
  const std::string& filename = (argc == 2) ? argv[1] : DEFAULT_PATH;
  ConfigParser config_parser(filename);
  Config config = config_parser.parse();

  return config;
}

ServerManager setServer(int argc, char** argv) {
  checkArgs(argc);
  printLogo();
  registerSignalHandlers();

  const Config config = createConfig(argc, argv);
  ServerManager manager(config);
  manager.setServer();

  return manager;
}

void runServer(ServerManager manager) {
  try {
    manager.runServer();
  } catch (std::runtime_error& e) {
  }
}
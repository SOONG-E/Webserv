#include "bootServer.hpp"

/* check that argc is 1 or 2 */
static void checkArgs(int argc) {
  if (argc > 2) {
    Error::log(Error::INFO[EARG], "", EXIT_FAILURE);
  }
}

/* print a logo of the server */
static void printLogo() {
  std::cout << readFile("./static/Webserv.art") << std::endl;
}

/* handle a signals */
static void registerSignalHandlers() {
  signal(SIGPIPE, SIG_IGN);
  signal(SIGCHLD, SIG_IGN);
}

/* parse configuration file and create Config instance */
static Config createConfig(int argc, char** argv) {
  const std::string& filename = (argc == 2) ? argv[1] : DEFAULT_PATH;
  ConfigParser config_parser(filename);
  Config config = config_parser.parse();

  return config;
}

/* set ServerManager with Config and register the servers
(bind socket, open socket to listen) */
ServerManager setServer(int argc, char** argv) {
  checkArgs(argc);
  printLogo();
  registerSignalHandlers();

  const Config config = createConfig(argc, argv);
  ServerManager manager(config);
  try {
    manager.setServer();
  } catch (std::runtime_error& e) {
    Error::log(Error::INFO[ESYSTEM], e.what(), EXIT_FAILURE);
  }

  return manager;
}

/* run Server */
void runServer(ServerManager manager) {
  try {
    manager.runServer();
  } catch (std::runtime_error& e) {
    Error::log(Error::INFO[ESYSTEM], e.what(), EXIT_FAILURE);
  }
}
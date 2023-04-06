#include "Listen.hpp"

#include <cstdlib>

#include "Error.hpp"
#include "constant.hpp"
#include "utility.hpp"

const std::string Listen::DEFAULT_HOST = "0.0.0.0";

Listen::Listen() : host(DEFAULT_HOST), port(DEFAULT_PORT) {}

Listen::Listen(const std::string& token)
    : host(DEFAULT_HOST), port(DEFAULT_PORT) {
  std::vector<std::string> splitted = split(token, ":");
  if (splitted.size() == 1) {
    if (isNumber(splitted[0])) {
      port = splitted[0];
    } else {
      host = splitted[0];
    }
  } else if (splitted.size() == 2) {
    host = splitted[0];
    port = splitted[1];
  } else {
    Error::log(Error::INFO[ETOKEN], "", EXIT_FAILURE);
  }
  server_socket_key = host + ":" + port;
}

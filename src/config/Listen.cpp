#include "Listen.hpp"

#include <cstdlib>

#include "Error.hpp"
#include "constant.hpp"
#include "utility.hpp"

const std::string Listen::DEFAULT_IP = "0.0.0.0";

Listen::Listen() : ip(DEFAULT_IP), port(DEFAULT_PORT) {}

Listen::Listen(const std::string& token) : ip(DEFAULT_IP), port(DEFAULT_PORT) {
  std::vector<std::string> splitted = split(token, ":");
  if (splitted.size() == 1) {
    if (isNumber(splitted[0])) {
      port = splitted[0];
    } else {
      ip = splitted[0];
    }
  } else if (splitted.size() == 2) {
    ip = splitted[0];
    port = splitted[1];
  } else {
    Error::log(Error::INFO[ETOKEN], "", EXIT_FAILURE);
  }
  server_socket_key = ip + ":" + port;
}

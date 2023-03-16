#ifndef LISTEN_HPP_
#define LISTEN_HPP_

#include "constant.hpp"
#include "exception.hpp"
#include "utility.hpp"

struct Listen {
  explicit Listen(const std::string& token)
      : host(kDefaults[kHost]), port(kDefaults[kPort]) {
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
      throw ConfigException(kErrors[kToken]);
    }
    socket_key = host + ":" + port;
  }

  std::string host;
  std::string port;
  std::string socket_key;
};

#endif

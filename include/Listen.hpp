#ifndef LISTEN_HPP_
#define LISTEN_HPP_

#include "constant.hpp"
#include "exception.hpp"
#include "utility.hpp"

struct Listen {
  explicit Listen(const std::string& token)
      : raw(token), host(kDefaults[kHost]) {
    std::vector<std::string> splitted = split(token, ":");
    if (splitted.size() == 1) {
      port = splitted[0];
    } else if (splitted.size() == 2) {
      host = splitted[0];
      port = splitted[1];
    } else {
      throw ConfigException(kErrors[kToken]);
    }
  }

  std::string raw;
  std::string host;
  std::string port;
};

#endif

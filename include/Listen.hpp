#ifndef LISTEN_HPP_
#define LISTEN_HPP_

#include "constant.hpp"
#include "exception.hpp"
#include "utility.hpp"

struct Listen {
  enum Index {
    HOST,
    PORT,
  };

  static const std::string DEFAULTS[];

  Listen();
  explicit Listen(const std::string& token);

  std::string host;
  std::string port;
  std::string socket_key;
};

#endif

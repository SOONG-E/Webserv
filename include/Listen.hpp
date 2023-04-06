#ifndef LISTEN_HPP_
#define LISTEN_HPP_

#include <string>

struct Listen {
  static const std::string DEFAULT_HOST;

  Listen();
  explicit Listen(const std::string& token);

  std::string host;
  std::string port;
  std::string server_socket_key;
};

#endif

#ifndef LISTEN_HPP_
#define LISTEN_HPP_

#include <string>

struct Listen {
  static const std::string DEFAULT_IP;

  Listen();
  explicit Listen(const std::string& token);

  std::string ip;
  std::string port;
  std::string server_socket_key;
};

#endif

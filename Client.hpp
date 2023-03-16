#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

#include "InetSocketAddress.hpp"

class Client {
 public:
  Client(int socket, const InetSocketAddress& address);
  Client(const Client& src);
  ~Client();

  Client& operator=(const Client& src);

  int getSocket() const;
  const std::string& getSocketKey() const;

 private:
  int _socket;
  std::string _socket_key;
};

#endif
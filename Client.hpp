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
  const std::string& getKey() const;

 private:
  int _socket;
  std::string _server_blocks_key;
};

#endif
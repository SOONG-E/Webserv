#ifndef SOCKET_ADDRESS_HPP
#define SOCKET_ADDRESS_HPP

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <string>

#include "utility.hpp"

class SocketAddress {
 public:
  SocketAddress();
  SocketAddress(const std::string &host, const std::string &port);
  SocketAddress(const sockaddr &address, const socklen_t address_len);
  SocketAddress(const SocketAddress &src);
  ~SocketAddress();

  SocketAddress &operator=(const SocketAddress &src);

  sockaddr &getAddress();
  const sockaddr &getAddress() const;
  socklen_t getAddressLen() const;
  const std::string &getIP() const;
  const std::string &getPort() const;

 private:
  sockaddr address_;
  socklen_t address_len_;
  std::string ip_;
  std::string port_;
};

#endif
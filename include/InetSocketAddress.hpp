#ifndef INET_SOCKET_ADDRESS_HPP
#define INET_SOCKET_ADDRESS_HPP

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <string>

#include "utility.hpp"

class InetSocketAddress {
 public:
  InetSocketAddress(const std::string &host, const std::string &port);
  InetSocketAddress(const sockaddr &address, const socklen_t address_len);
  InetSocketAddress(const InetSocketAddress &src);
  ~InetSocketAddress();

  InetSocketAddress &operator=(const InetSocketAddress &src);

  sockaddr &getAddress();
  const sockaddr &getAddress() const;
  socklen_t getAddressLen() const;
  const std::string &getIP() const;
  const std::string &getPort() const;

 private:
  sockaddr _address;
  socklen_t _address_len;
  std::string _ip;
  std::string _port;
};

#endif
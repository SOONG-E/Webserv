#ifndef INET_SOCKET_ADDRESS_HPP
#define INET_SOCKET_ADDRESS_HPP

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <string>

class InetSocketAddress {
 public:
  InetSocketAddress(const std::string &port);
  InetSocketAddress(const std::string &host, const std::string &port);
  InetSocketAddress(const InetSocketAddress &src);
  ~InetSocketAddress();

  InetSocketAddress &operator=(const InetSocketAddress &src);

  sockaddr &getAddress();
  const sockaddr &getAddress() const;
  socklen_t getAddressLen() const;

 private:
  sockaddr addr;
  socklen_t addrlen;
};

#endif
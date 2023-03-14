#ifndef INET_SOCKET_ADDRESS_HPP
#define INET_SOCKET_ADDRESS_HPP

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <string>

class InetSocketAddress {
 public:
  InetSocketAddress(const std::string &port = "80");
  InetSocketAddress(const std::string &host, const std::string &port);
  InetSocketAddress(const sockaddr_in &addr, const socklen_t &addr_len);
  InetSocketAddress(const InetSocketAddress &src);
  ~InetSocketAddress();

  InetSocketAddress &operator=(const InetSocketAddress &src);

  sockaddr_in &getAddress();
  const sockaddr_in &getAddress() const;
  socklen_t getAddressLen() const;
  int getIP() const;
  int getPort() const;

 private:
  sockaddr_in addr;
  socklen_t addr_len;
};

#endif
#include "InetSocketAddress.hpp"

InetSocketAddress::InetSocketAddress(const std::string &port = "80") {
  struct addrinfo hints, *addr_info;

  memset(&hints, 0, sizeof(hints));

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  int gai_result = getaddrinfo(NULL, port.c_str(), &hints, &addr_info);

  if (gai_result != 0) throw std::invalid_argument(gai_strerror(gai_result));

  addr = *((sockaddr_in *)addr_info->ai_addr);
  addr_len = addr_info->ai_addrlen;

  freeaddrinfo(addr_info);
}

InetSocketAddress::InetSocketAddress(const std::string &ip,
                                     const std::string &port = "80") {
  struct addrinfo hints, *addr_info;

  memset(&hints, 0, sizeof(hints));

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  int gai_result = getaddrinfo(ip.c_str(), port.c_str(), &hints, &addr_info);

  if (gai_result != 0) throw std::invalid_argument(gai_strerror(gai_result));

  addr = *((sockaddr_in *)addr_info->ai_addr);
  addr_len = addr_info->ai_addrlen;

  freeaddrinfo(addr_info);
}

InetSocketAddress::InetSocketAddress(const sockaddr_in &addr,
                                     const socklen_t &addr_len)
    : addr(addr), addr_len(addr_len) {}

InetSocketAddress::InetSocketAddress(const InetSocketAddress &src) {
  *this = src;
}

InetSocketAddress &InetSocketAddress::operator=(const InetSocketAddress &src) {
  if (this != &src) {
    addr = src.addr;
    addr_len = src.addr_len;
  }
  return *this;
}

InetSocketAddress::~InetSocketAddress() {}

sockaddr_in &InetSocketAddress::getAddress() { return addr; }

const sockaddr_in &InetSocketAddress::getAddress() const { return addr; }

socklen_t InetSocketAddress::getAddressLen() const { return addr_len; }

int InetSocketAddress::getIP() const { return ntohl(addr.sin_addr.s_addr); }

int InetSocketAddress::getPort() const { return ntohs(addr.sin_port); }
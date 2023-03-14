#include "InetSocketAddress.hpp"

InetSocketAddress::InetSocketAddress(const std::string &port = "80") {
  struct addrinfo hints, *addr_info;

  memset(&hints, 0, sizeof(hints));

  hints.ai_family = AF_INET;
  // hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  int gai_result = getaddrinfo(NULL, port.c_str(), &hints, &addr_info);

  if (gai_result != 0) throw std::invalid_argument(gai_strerror(gai_result));

  addr = *(addr_info->ai_addr);
  addrlen = addr_info->ai_addrlen;

  freeaddrinfo(addr_info);
}

InetSocketAddress::InetSocketAddress(const std::string &host,
                                     const std::string &port = "80") {
  struct addrinfo hints, *addr_info;

  memset(&hints, 0, sizeof(hints));

  hints.ai_family = AF_INET;
  // hints.ai_socktype = SOCK_STREAM;

  int gai_result = getaddrinfo(host.c_str(), port.c_str(), &hints, &addr_info);

  if (gai_result != 0) throw std::invalid_argument(gai_strerror(gai_result));

  addr = *(addr_info->ai_addr);
  addrlen = addr_info->ai_addrlen;

  freeaddrinfo(addr_info);
}

InetSocketAddress::InetSocketAddress(const InetSocketAddress &src) {
  *this = src;
}

InetSocketAddress &InetSocketAddress::operator=(const InetSocketAddress &src) {
  if (this != &src) {
    addr = src.addr;
    addrlen = src.addrlen;
  }
  return *this;
}

InetSocketAddress::~InetSocketAddress() {}

sockaddr &InetSocketAddress::getAddr() { return addr; }

const sockaddr &InetSocketAddress::getAddr() const { return addr; }

socklen_t InetSocketAddress::getAddrLen() const { return addrlen; }
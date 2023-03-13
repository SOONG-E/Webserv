#include "InetSocketAddress.hpp"

InetSocketAddress::InetSocketAddress(const std::string &port = "80") {
  struct addrinfo hints, *addr_info;

  memset(&hints, 0, sizeof(hints));

  hints.ai_family = AF_INET;
  // hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  int gai_result = getaddrinfo(NULL, port.c_str(), &hints, &addr_info);

  if (gai_result != 0) throw std::invalid_argument(gai_strerror(gai_result));

  addr->sa_family = addr_info->ai_addr->sa_family;
  addr->sa_len = addr_info->ai_addr->sa_len;
  for (int i = 0; i < sizeof(addr->sa_data); i++)
    addr->sa_data[i] = addr_info->ai_addr->sa_data[i];
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

  addr->sa_family = addr_info->ai_addr->sa_family;
  addr->sa_len = addr_info->ai_addr->sa_len;
  strncpy(addr->sa_data, addr_info->ai_addr->sa_data, sizeof(addr->sa_data));
  addrlen = addr_info->ai_addrlen;

  freeaddrinfo(addr_info);
}

InetSocketAddress::InetSocketAddress(const InetSocketAddress &src) {
  *this = src;
}

InetSocketAddress &InetSocketAddress::operator=(const InetSocketAddress &src) {
  if (this != &src) {
    addrlen = src.addrlen;
    addr->sa_family = src.addr->sa_family;
    addr->sa_len = src.addr->sa_len;
    strncpy(addr->sa_data, src.addr->sa_data, sizeof(addr->sa_data));
  }
  return *this;
}

InetSocketAddress::~InetSocketAddress() { delete addr; }

sockaddr *InetSocketAddress::getAddr() const { return addr; }

socklen_t InetSocketAddress::getAddrLen() const { return addrlen; }
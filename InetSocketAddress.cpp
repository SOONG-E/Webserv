#include "InetSocketAddress.hpp"

InetSocketAddress::InetSocketAddress(const std::string &port = "80") {
  struct addrinfo hints, *addr_info;

  memset(&hints, 0, sizeof(hints));

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  int gai_result = getaddrinfo(NULL, port.c_str(), &hints, &addr_info);

  if (gai_result != 0) throw std::invalid_argument(gai_strerror(gai_result));

  addr = *(addr_info->ai_addr);
  addr_len = addr_info->ai_addrlen;

  freeaddrinfo(addr_info);

  this->ip = "0.0.0.0";
  this->port = port;
}

InetSocketAddress::InetSocketAddress(const std::string &ip,
                                     const std::string &port = "80") {
  struct addrinfo hints, *addr_info;

  memset(&hints, 0, sizeof(hints));

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  int gai_result = getaddrinfo(ip.c_str(), port.c_str(), &hints, &addr_info);

  if (gai_result != 0) throw std::invalid_argument(gai_strerror(gai_result));

  addr = *(addr_info->ai_addr);
  addr_len = addr_info->ai_addrlen;

  freeaddrinfo(addr_info);

  this->ip = ip;
  this->port = port;
}

InetSocketAddress::InetSocketAddress(const InetSocketAddress &src) {
  *this = src;
}

InetSocketAddress &InetSocketAddress::operator=(const InetSocketAddress &src) {
  if (this != &src) {
    addr = src.addr;
    addr_len = src.addr_len;
    ip = src.ip;
    port = src.port;
  }
  return *this;
}

InetSocketAddress::~InetSocketAddress() {}

sockaddr &InetSocketAddress::getAddress() { return addr; }

const sockaddr &InetSocketAddress::getAddress() const { return addr; }

socklen_t InetSocketAddress::getAddressLen() const { return addr_len; }

const std::string &InetSocketAddress::getIP() const { return ip; }

const std::string &InetSocketAddress::getPort() const { return port; }
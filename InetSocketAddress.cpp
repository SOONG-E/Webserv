#include "InetSocketAddress.hpp"

InetSocketAddress::InetSocketAddress(const std::string &ip,
                                     const std::string &port) {
  struct addrinfo hints, *addr_info;

  memset(&hints, 0, sizeof(hints));

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  int gai_result = getaddrinfo(ip.c_str(), port.c_str(), &hints, &addr_info);

  if (gai_result != 0) throw std::invalid_argument(gai_strerror(gai_result));

  _address = *(addr_info->ai_addr);
  _address_len = addr_info->ai_addrlen;

  freeaddrinfo(addr_info);

  _ip = ip;
  _port = port;
}

InetSocketAddress::InetSocketAddress(const InetSocketAddress &src) {
  *this = src;
}

InetSocketAddress &InetSocketAddress::operator=(const InetSocketAddress &src) {
  if (this != &src) {
    _address = src._address;
    _address_len = src._address_len;
    _ip = src._ip;
    _port = src._port;
  }
  return *this;
}

InetSocketAddress::~InetSocketAddress() {}

sockaddr &InetSocketAddress::getAddress() { return _address; }

const sockaddr &InetSocketAddress::getAddress() const { return _address; }

socklen_t InetSocketAddress::getAddressLen() const { return _address_len; }

const std::string &InetSocketAddress::getIP() const { return _ip; }

const std::string &InetSocketAddress::getPort() const { return _port; }
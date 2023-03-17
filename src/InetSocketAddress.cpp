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

  _ip = ip;
  _port = port;
}

InetSocketAddress::InetSocketAddress(const sockaddr &address,
                                     const socklen_t address_len) {
  _address = address;
  _address_len = address_len;

  sockaddr_in addr_in = *((sockaddr_in *)&_address);

  for (int i = 3; i >= 0; --i) {
    _ip += toString((ntohl(addr_in.sin_addr.s_addr) >> (i * 8)) & 0xFF);
    if (i == 0) break;
    _ip += ".";
  }
  _port = toString(ntohs(addr_in.sin_port));
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
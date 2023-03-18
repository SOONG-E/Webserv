#include "SocketAddress.hpp"

SocketAddress::SocketAddress() : _address_len(0) {
  memset(&_address, 0, sizeof(_address));
}

SocketAddress::SocketAddress(const std::string &ip, const std::string &port) {
  struct addrinfo hints, *addr_info;

  memset(&hints, 0, sizeof(hints));

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  int result = getaddrinfo(ip.c_str(), port.c_str(), &hints, &addr_info);

  if (result != 0) throw std::invalid_argument(gai_strerror(result));

  _address = *(addr_info->ai_addr);
  _address_len = addr_info->ai_addrlen;

  _ip = ip;
  _port = port;

  freeaddrinfo(addr_info);
}

SocketAddress::SocketAddress(const sockaddr &address,
                             const socklen_t address_len) {
  _address = address;
  _address_len = address_len;

  sockaddr_in addr_in = *(reinterpret_cast<sockaddr_in *>(&_address));

  for (int i = 3; i >= 0; --i) {
    _ip += toString((ntohl(addr_in.sin_addr.s_addr) >> (i * 8)) & 0xFF);
    if (i == 0) break;
    _ip += ".";
  }
  _port = toString(ntohs(addr_in.sin_port));
}

SocketAddress::SocketAddress(const SocketAddress &src) { *this = src; }

SocketAddress &SocketAddress::operator=(const SocketAddress &src) {
  if (this != &src) {
    _address = src._address;
    _address_len = src._address_len;
    _ip = src._ip;
    _port = src._port;
  }
  return *this;
}

SocketAddress::~SocketAddress() {}

sockaddr &SocketAddress::getAddress() { return _address; }

const sockaddr &SocketAddress::getAddress() const { return _address; }

socklen_t SocketAddress::getAddressLen() const { return _address_len; }

const std::string &SocketAddress::getIP() const { return _ip; }

const std::string &SocketAddress::getPort() const { return _port; }
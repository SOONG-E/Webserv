#include "SocketAddress.hpp"

#include <netdb.h>

#include <cstring>
#include <stdexcept>

#include "utility.hpp"

SocketAddress::SocketAddress(const sockaddr& address,
                             const socklen_t address_len)
    : address_(address), address_len_(address_len) {
  sockaddr_in addr_in = *(reinterpret_cast<sockaddr_in*>(&address_));

  for (int i = 3; i >= 0; --i) {
    ip_ += toString((ntohl(addr_in.sin_addr.s_addr) >> (i * 8)) & 0xFF);
    if (i == 0) {
      break;
    }
    ip_ += ".";
  }
  port_ = toString(ntohs(addr_in.sin_port));
}

SocketAddress::SocketAddress(const SocketAddress& src) { *this = src; }

SocketAddress& SocketAddress::operator=(const SocketAddress& src) {
  if (this != &src) {
    address_ = src.address_;
    address_len_ = src.address_len_;
    ip_ = src.ip_;
    port_ = src.port_;
  }
  return *this;
}

SocketAddress::~SocketAddress() {}

sockaddr& SocketAddress::getAddress() { return address_; }

const sockaddr& SocketAddress::getAddress() const { return address_; }

socklen_t SocketAddress::getAddressLen() const { return address_len_; }

const std::string& SocketAddress::getIP() const { return ip_; }

const std::string& SocketAddress::getPort() const { return port_; }

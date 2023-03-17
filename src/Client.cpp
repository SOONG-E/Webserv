#include "Client.hpp"

Client::Client(int socket, const InetSocketAddress& address) {
  _socket = socket;
  _address = address;
}

Client::Client(const Client& src) { *this = src; }

Client::~Client() {}

Client& Client::operator=(const Client& src) {
  _socket = src._socket;
  _address = src._address;
  _request = src._request;

  return *this;
}

int Client::getSocket() const { return _socket; }

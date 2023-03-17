#include "Client.hpp"

Client::Client(int socket, const InetSocketAddress& serv_addr) {
  _socket = socket;
  _socket_key = serv_addr.getIP() + ":" + serv_addr.getPort();
}

Client::Client(const Client& src) { *this = src; }

Client::~Client() {}

Client& Client::operator=(const Client& src) {
  _socket = src._socket;
  _socket_key = src._socket_key;
  return *this;
}

int Client::getSocket() const { return _socket; }

const std::string& Client::getSocketKey() const { return _socket_key; }
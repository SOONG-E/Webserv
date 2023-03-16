#include "Client.hpp"

Client::Client(int socket, const InetSocketAddress& serv_addr) {
  _socket = socket;
  _server_blocks_key = serv_addr.getIP() + ":" + serv_addr.getPort();
}

Client::Client(const Client& src) { *this = src; }

Client::~Client() {}

Client& Client::operator=(const Client& src) {
  _socket = src._socket;
  _server_blocks_key = src._server_blocks_key;
  return *this;
}

int Client::getSocket() const { return _socket; }

const std::string& Client::getKey() const { return _server_blocks_key; }
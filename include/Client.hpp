#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

#include "HttpParser.hpp"
#include "HttpRequest.hpp"
#include "InetSocketAddress.hpp"

class Client {
 public:
  Client(int socket, const InetSocketAddress& address);
  Client(const Client& src);
  ~Client();

  Client& operator=(const Client& src);

  int getSocket() const;

 private:
  int _socket;
  InetSocketAddress _address;
  HttpRequest _request;
};

#endif
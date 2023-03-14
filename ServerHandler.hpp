#ifndef SERVER_HANDLER_HPP
#define SERVER_HANDLER_HPP

#include <map>
#include "SocketChannel.hpp"
#include "Selector.hpp"

class ServerHandler {
 public:
 // config에 들어잇는 server들 전부 세팅하고 그걸 토대로 socket_channel 다 만들고 시작..
  ServerHandler(const Config& config);
  ServerHandler(const ServerHandler &src);
  ~ServerHandler();

  ServerHandler &operator=(const ServerHandler &src);

 private:
  //자료구조.. socket_channels;
  //자료구조.. servers;
  Selector selector;
};

#endif
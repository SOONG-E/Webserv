#ifndef HTTP_SERVER_HPP_
#define HTTP_SERVER_HPP_

#include <map>
#include <vector>

#include "ServerBlock.hpp"

class HttpServer {
 public:
  HttpServer(ServerBlock &server_block);
  ~HttpServer();

 private:
  std::vector<Location> locations_;
  std::map<std::string, std::string> error_pages_;
};

#endif
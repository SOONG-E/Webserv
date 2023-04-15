#ifndef HTTP_SERVER_HPP_
#define HTTP_SERVER_HPP_

#include <map>
#include <vector>

#include "ServerBlock.hpp"

class HttpServer {
 public:
  typedef std::vector<Location> LocationType;
  typedef std::map<std::string, std::string> ErrorPageType;

  HttpServer(const ServerBlock &server_block);
  ~HttpServer();

 private:
  const LocationType locations_;
  const ErrorPageType error_pages_;
};

#endif
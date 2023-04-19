#ifndef HTTP_SERVER_HPP_
#define HTTP_SERVER_HPP_

#include <map>
#include <vector>

#include "ServerBlock.hpp"
#include "constant.hpp"
#include "exception.hpp"

class HttpServer {
 public:
  typedef std::vector<Location> LocationType;
  typedef std::map<std::string, std::string> ErrorPageType;

  HttpServer(const ServerBlock &server_block);
  ~HttpServer();

  const Location &findLocation(const std::string &request_uri) const;
  std::size_t find(const std::string &uri) const;
  const Location &redirect(const Location &location) const;
  const std::string &getErrorPage(const std::string &code) const;

 private:
  const LocationType locations_;
  const ErrorPageType error_pages_;
};

#endif

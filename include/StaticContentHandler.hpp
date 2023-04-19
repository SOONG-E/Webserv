#ifndef STATIC_CONTENT_HANDLER_HPP_
#define STATIC_CONTENT_HANDLER_HPP_

#include <unistd.h>

#include "Client.hpp"

class StaticContentHandler {
 public:
  static struct Response handle(Client *client);

 private:
  StaticContentHandler(){};
  ~StaticContentHandler(){};

  static std::string generateBody(Client *client);
  static std::string loadErrorPage(HttpServer *http_server, const int status);
  static void deleteFile(Client *client, const std::string &uri);
};

#endif
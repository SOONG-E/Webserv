#ifndef SESSION_HANDLER_HPP_
#define SESSION_HANDLER_HPP_

#include "Client.hpp"

class SessionHandler {
 public:
  static bool recognizeRequest(HttpRequest &request);
  static struct Response handle(Client *client);

 private:
  static std::string getNewSessionId(HttpServer *server);
  static std::string createSessionId(void);
  static void createSession(Client *client, std::string &id);
  static Session::ValueType parseData(const std::string &data);
  SessionHandler(){};
  ~SessionHandler(){};
};

#endif
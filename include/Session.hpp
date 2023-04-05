#ifndef SESSION_HPP_
#define SESSION_HPP_

#include <ctime>
#include <vector>

#include "constant.hpp"

class Client;

class Session {
 public:
  Session(const std::string& id, Client& client);
  Session(const Session& src);
  ~Session();

  const std::string& getID() const;
  Client* getClient();
  void setClient(Client* client);
  time_t getTimeout() const;
  void setTimeout(std::time_t time = std::time(NULL));

 private:
  const std::string id_;
  Client* client_;
  time_t timeout_;
};

#endif

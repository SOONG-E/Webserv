#ifndef SESSION_HPP_
#define SESSION_HPP_

#include <ctime>
#include <vector>

#include "constant.hpp"

class Client;

class Session {
 public:
  Session(const std::string& id, Client* client);
  Session(const Session& src);
  ~Session();

  const std::string& getID() const;
  time_t getTimeout() const;
  void setTimeout(std::time_t time = std::time(NULL));
  Client* getClient();
  void setClient(Client* client);

 private:
  const std::string id_;
  time_t timeout_;
  Client* client_;
};

#endif

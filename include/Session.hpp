#ifndef SESSION_HPP_
#define SESSION_HPP_

#include <ctime>
#include <vector>

#include "constant.hpp"

class Client;

class Session {
 public:
  typedef std::map<std::string, std::string> ValueType;

  Session(const std::string& id);
  Session(const std::string& id, ValueType values);
  Session(const Session& origin);
  Session operator=(const Session& origin);
  ~Session();

  void setTimeout(std::time_t time = std::time(NULL));

  const std::string& getID() const;
  std::time_t getTimeout() const;
  const std::string& getValue(std::string key) const;

 private:
  const std::string id_;
  ValueType values_;
  std::time_t timeout_;
};

#endif

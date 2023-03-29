#ifndef SESSION_HPP_
#define SESSION_HPP_

#include <string>

class Session {
 public:
  Session(std::string id);
  Session(Session &origin);
  Session operator=(Session &origin);
  ~Session();

  const std::string &getId(void) const;

 private:
  std::string id_;
};

#endif
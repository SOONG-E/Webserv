#ifndef SESSION_HPP_
#define SESSION_HPP_

#include <string>

class Session {
 public:
  Session();
  Session(std::string id);
  Session(const Session &origin);
  Session& operator=(const Session &origin);
  ~Session();

  const std::string &getId(void) const;

 private:
  std::string id_;
};

#endif
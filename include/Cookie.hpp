#ifndef COOKIE_HPP_
#define COOKIE_HPP_

#include <map>

#include "Session.hpp"

class Cookie {
 public:
  std::string createCookie(std::string &id);
  void destroyCookie(std::string &session_id);
  const std::string &getClientId(std::string &session_id);
  const Session &getSession(std::string &session_id);

 private:
  typedef std::map<std::string, Session> session_type;

  session_type session_;
};

#endif

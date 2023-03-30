#ifndef COOKIE_HPP_
#define COOKIE_HPP_

#include <functional>
#include <map>

#include "Session.hpp"

class Cookie {
 public:
  std::string createCookie(std::string &id);
  void destroyCookie(std::string &session_id);
  const std::string &getClientId(std::string &session_id);
  Session &getSession(std::string &session_id);

 private:
  typedef std::map<std::string, Session> session_type;

  std::string createHash(std::string id);
  session_type session_;
  std::hash<std::string> hash_;
};

#endif
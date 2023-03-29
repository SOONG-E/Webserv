#ifndef COOKIE_HPP_
#define COOKIE_HPP_

#include <functional>
#include <map>

#include "Session.hpp"

class Cookie {
 public:
  static std::string createCookie(std::string &id);
  static void destroyCookie(std::string &session_id);
  static const std::string &getClientId(std::string &session_id);
  static Session &getSession(std::string &session_id);

 private:
  typedef std::map<std::string, Session> session_type;

  static session_type session_;
  static std::hash<std::string> hash_;
};

#endif
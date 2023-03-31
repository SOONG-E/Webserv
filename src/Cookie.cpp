#include "Cookie.hpp"

#include "utility.hpp"

std::string Cookie::createCookie(std::string &id) {
  (void)id;
  return "";
}  // 수정

void Cookie::destroyCookie(std::string &session_id) {
  session_.erase(session_id);
}

const std::string &Cookie::getClientId(std::string &session_id) {
  const Session &session = getSession(session_id);

  return session.getId();
}

const Session &Cookie::getSession(std::string &session_id) {
  return session_[session_id];
  // 없으면..?
}

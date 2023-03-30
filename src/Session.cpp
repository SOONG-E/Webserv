#include "Session.hpp"

Session::Session() {}

Session::Session(std::string id) : id_(id) {}

Session::Session(const Session &origin) : id_(origin.id_) {}

Session& Session::operator=(const Session &origin) {
  if (this != &origin) {
    id_ = origin.id_;
  }
  return *this;
}

Session::~Session() {}

const std::string &Session::getId(void) const { return id_; }

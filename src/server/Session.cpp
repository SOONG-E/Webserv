#include "Session.hpp"

Session::Session(const std::string& id) : id_(id) {}

Session::Session(const std::string& id, ValueType values)
    : id_(id), values_(values) {}

Session::Session(const Session& origin)
    : id_(origin.id_), values_(origin.values_), timeout_(origin.timeout_) {}

Session Session::operator=(const Session& origin) { return Session(origin); }

Session::~Session() {}

/*======================//
 Setter
========================*/

void Session::setTimeout(std::time_t time) {
  timeout_ = time + SESSION_TIMEOUT;
}

/*======================//
 Getter
========================*/

const std::string& Session::getID() const { return id_; }

std::time_t Session::getTimeout() const { return timeout_; }

const std::string& Session::getValue(std::string key) const {
  ValueType::const_iterator value = values_.find(key);

  if (value == values_.end()) {
    return EMPTY_STRING;
  }
  return value->second;
}

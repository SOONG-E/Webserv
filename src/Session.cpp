#include "Session.hpp"

Session::Session(const std::string& id, Client& client)
    : id_(id), client_(client) {
  setTimeout();
}

Session::Session(const Session& src)
    : id_(src.id_), client_(src.client_), timeout_(src.timeout_) {}

Session::~Session() {}

Client& Session::getClient() { return client_; }

const std::string& Session::getID() const { return id_; }

time_t Session::getTimeout() const { return timeout_; }

void Session::setTimeout(time_t time) { timeout_ = time + SESSION_TIMEOUT; }
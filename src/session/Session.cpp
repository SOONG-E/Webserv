#include "Session.hpp"

Session::Session(const std::string& id, Client* client)
    : id_(id), client_(client) {
  setTimeout();
}

Session::Session(const Session& src)
    : id_(src.id_), timeout_(src.timeout_), client_(src.client_) {}

Session::~Session() {}

const std::string& Session::getID() const { return id_; }

std::time_t Session::getTimeout() const { return timeout_; }

void Session::setTimeout(std::time_t time) { timeout_ = time + SESSION_TIMEOUT; }

Client* Session::getClient() { return client_; }

void Session::setClient(Client* client) { client_ = client; }
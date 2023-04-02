#include "Session.hpp"

Session::Session(const std::string& session_id) : session_id_(session_id) {
  setTimeout();
}

Session::Session(const Session& src)
    : session_id_(src.session_id_),
      timeout_(src.timeout_),
      upload_filenames_(src.upload_filenames_) {}

Session::~Session() {}

const std::string& Session::getSessionID() const { return session_id_; }

time_t Session::getTimeout() const { return timeout_; }

void Session::setTimeout(time_t time) { timeout_ = time + SESSION_TIMEOUT; }

void Session::addFileName(const std::string& filename) {
  upload_filenames_.push_back(filename);
}

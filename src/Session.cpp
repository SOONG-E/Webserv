#include "Session.hpp"

Session::Session(const std::string& id) : id_(id) { setTimeout(); }

Session::Session(const Session& src)
    : id_(src.id_),
      timeout_(src.timeout_),
      upload_filenames_(src.upload_filenames_) {}

Session::~Session() {}

const std::string& Session::getID() const { return id_; }

time_t Session::getTimeout() const { return timeout_; }

void Session::setTimeout(time_t time) { timeout_ = time + SESSION_TIMEOUT; }

void Session::addFilename(const std::string& filename) {
  upload_filenames_.push_back(filename);
}

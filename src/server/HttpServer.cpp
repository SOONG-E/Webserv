#include "HttpServer.hpp"

HttpServer::HttpServer(const int id, const ServerBlock& server_block)
    : server_id_(id),
      locations_(server_block.locations),
      error_pages_(server_block.error_pages) {}

HttpServer::HttpServer(const HttpServer& origin)
    : server_id_(origin.server_id_),
      locations_(origin.locations_),
      error_pages_(origin.error_pages_),
      sessions_(origin.sessions_) {}

HttpServer HttpServer::operator=(const HttpServer& origin) {
  return HttpServer(origin);
}
HttpServer::~HttpServer() {}

const Location& HttpServer::findLocation(const std::string& request_uri) const {
  std::size_t index = find(request_uri);
  if (index != NPOS) {
    return redirect(locations_[index]);
  }
  std::size_t end_pos = request_uri.size();
  while (true) {
    end_pos = request_uri.rfind("/", end_pos - 1);
    if (end_pos == std::string::npos) {
      throw ResponseException(C404);
    }
    std::string partial_uri = request_uri.substr(0, end_pos + 1);
    index = find(partial_uri);
    if (index != NPOS) {
      return redirect(locations_[index]);
    }
  }
}

std::size_t HttpServer::find(const std::string& uri) const {
  for (std::size_t i = 0; i < locations_.size(); ++i) {
    if (uri == locations_[i].getUri()) {
      return i;
    }
  }
  return NPOS;
}

const Location& HttpServer::redirect(const Location& location) const {
  if (location.getReturnUrl().empty()) {
    return location;
  }
  return findLocation(location.getReturnUrl());
}

const std::string& HttpServer::getErrorPage(const std::string& code) const {
  if (error_pages_.find(code) == error_pages_.end()) {
    return EMPTY_STRING;
  }
  return error_pages_.at(code);
}

Session* HttpServer::getSession(const std::string& id) const {
  SessionType::const_iterator session = sessions_.find(id);

  if (session == sessions_.end()) {
    return NULL;
  }
  return session->second;
}

bool HttpServer::isExistSessionId(std::string& id) {
  if (sessions_.find(id) == sessions_.end()) {
    return false;
  }
  return true;
}

void HttpServer::addSession(std::string& id, Session* session) {
  sessions_[id] = session;
}

void HttpServer::destroySession(const std::string& id) { sessions_.erase(id); }
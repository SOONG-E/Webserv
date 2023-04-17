#include "HttpServer.hpp"

HttpServer::HttpServer(const ServerBlock& server_block)
    : locations_(server_block.locations),
      error_pages_(server_block.error_pages) {}

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

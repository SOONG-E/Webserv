#include "ServerBlock.hpp"

#include <iostream>

#include "HttpResponse.hpp"
#include "exception.hpp"

ServerBlock::ServerBlock() {}

ServerBlock::ServerBlock(const ServerBlock& origin)
    : listens_(origin.listens_),
      server_names_(origin.server_names_),
      error_pages_(origin.error_pages_),
      location_blocks_(origin.location_blocks_) {}

ServerBlock& ServerBlock::operator=(const ServerBlock& origin) {
  if (this != &origin) {
    listens_ = origin.listens_;
    server_names_ = origin.server_names_;
    error_pages_ = origin.error_pages_;
    location_blocks_ = origin.location_blocks_;
  }
  return *this;
}

ServerBlock::~ServerBlock() {}

const std::vector<Listen>& ServerBlock::getListens(void) const {
  return listens_;
}

const std::set<std::string>& ServerBlock::getServerNames(void) const {
  return server_names_;
}

const std::string& ServerBlock::getErrorPage(const std::string& code) const {
  try {
    return error_pages_.at(code);
  } catch (std::out_of_range& e) {
    return HttpResponse::DEFAULT_ERROR_PAGE;
  }
}

const std::vector<LocationBlock>& ServerBlock::getLocationBlocks(void) const {
  return location_blocks_;
}

void ServerBlock::addListen(const std::string& socket_key) {
  listens_.push_back(Listen(socket_key));
}

void ServerBlock::addServerName(const std::string& name) {
  server_names_.insert(name);
}

void ServerBlock::addErrorPage(const std::string& code,
                               const std::string& page) {
  error_pages_.insert(std::make_pair(code, page));
}

void ServerBlock::addLocationBlock(const LocationBlock& location_block) {
  location_blocks_.push_back(location_block);
}

const LocationBlock& ServerBlock::findLocationBlock(
    const std::string& request_uri) const {
  for (std::size_t i = 0; i < location_blocks_.size(); ++i) {
    if (request_uri == location_blocks_[i].getUri()) {
      return redirect(location_blocks_[i]);
    }
  }
  std::size_t end_pos = request_uri.size();
  while (true) {
    end_pos = request_uri.rfind("/", end_pos - 1);
    if (end_pos == std::string::npos) {
      throw ResponseException(C404);
    }
    std::string partial_uri = request_uri.substr(0, end_pos + 1);
    for (std::size_t i = 0; i < location_blocks_.size(); ++i) {
      if (partial_uri == location_blocks_[i].getUri()) {
        return redirect(location_blocks_[i]);
      }
    }
  }
}

const LocationBlock& ServerBlock::redirect(
    const LocationBlock& location) const {
  if (location.getReturnUrl().empty()) {
    return location;
  }
  return findLocationBlock(location.getReturnUrl());
}

std::set<std::string> ServerBlock::keys(void) const {
  std::set<std::string> keys;
  for (std::size_t i = 0; i < listens_.size(); ++i) {
    for (std::set<std::string>::const_iterator names_iter =
             server_names_.begin();
         names_iter != server_names_.end(); ++names_iter) {
      keys.insert(listens_[i].socket_key + ":" + *names_iter);
    }
  }
  return keys;
}

void ServerBlock::clear(void) {
  listens_.clear();
  server_names_.clear();
  error_pages_.clear();
  location_blocks_.clear();
}

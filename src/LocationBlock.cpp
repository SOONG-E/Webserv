#include "LocationBlock.hpp"

#include <cerrno>
#include <cstdlib>
#include <cstring>

#include "ByteUnits.hpp"
#include "constant.hpp"
#include "exception.hpp"

const std::string LocationBlock::DEFAULTS[] = {"1m", "html", "off",
                                               "index.html"};

LocationBlock::LocationBlock()
    : root_(DEFAULTS[ROOT]), autoindex_(DEFAULTS[AUTOINDEX]) {
  setBodyLimit(DEFAULTS[CLIENT_MAX_BODY_SIZE]);
  addAllowedMethod("GET");
  addAllowedMethod("POST");
  addIndex(DEFAULTS[INDEX]);
}

LocationBlock::LocationBlock(const LocationBlock& origin)
    : uri_(origin.uri_),
      body_limit_(origin.body_limit_),
      allowed_methods_(origin.allowed_methods_),
      return_url_(origin.return_url_),
      root_(origin.root_),
      autoindex_(origin.autoindex_),
      index_(origin.index_) {}

LocationBlock& LocationBlock::operator=(const LocationBlock& origin) {
  if (this != &origin) {
    uri_ = origin.uri_;
    body_limit_ = origin.body_limit_;
    allowed_methods_ = origin.allowed_methods_;
    return_url_ = origin.return_url_;
    root_ = origin.root_;
    autoindex_ = origin.autoindex_;
    index_ = origin.index_;
  }
  return *this;
}

LocationBlock::~LocationBlock() {}

bool LocationBlock::isImplementedMethod(const std::string& method) {
  for (std::size_t i = 0; i < METHODS_COUNT; ++i) {
    if (method == METHODS[i]) {
      return true;
    }
  }
  return false;
}

const std::string& LocationBlock::getUri(void) const { return uri_; }

std::size_t LocationBlock::getBodyLimit(void) const { return body_limit_; }

std::set<std::string>& LocationBlock::getAllowedMethods(void) {
  return allowed_methods_;
}

const std::string& LocationBlock::getReturnUrl(void) const {
  return return_url_;
}

const std::string& LocationBlock::getRoot(void) const { return root_; }

std::set<std::string>& LocationBlock::getIndex(void) { return index_; }

const std::set<std::string>& LocationBlock::getIndex(void) const {
  return index_;
}

void LocationBlock::setUri(const std::string& uri) { uri_ = uri; }

void LocationBlock::setBodyLimit(const std::string& raw) {
  static const ByteUnits UNITS;
  char* unit;
  body_limit_ = std::strtoul(raw.c_str(), &unit, 10);
  if (errno == ERANGE) {
    throw ConfigException(std::strerror(errno));
  }
  if (*unit == '\0') return;
  try {
    body_limit_ *= UNITS.size.at(unit);
  } catch (std::out_of_range& e) {
    throw ConfigException(ERRORS[TOKEN]);
  }
}

void LocationBlock::addAllowedMethod(const std::string& method) {
  allowed_methods_.insert(method);
}

void LocationBlock::setReturnUrl(const std::string& return_url) {
  return_url_ = return_url;
}

void LocationBlock::setRoot(const std::string& root) { root_ = root; }

void LocationBlock::setAutoindex(const std::string& autoindex) {
  autoindex_ = autoindex;
}

void LocationBlock::addIndex(const std::string& index) { index_.insert(index); }

bool LocationBlock::isAllowedMethod(const std::string& method) const {
  return allowed_methods_.find(method) != allowed_methods_.end();
}

void LocationBlock::clear(void) { *this = LocationBlock(); }

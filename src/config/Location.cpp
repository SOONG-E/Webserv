#include "Location.hpp"

#include <cerrno>
#include <cstdlib>
#include <cstring>

#include "ByteUnit.hpp"
#include "Error.hpp"
#include "constant.hpp"

const std::string Location::DEFAULTS[] = {
    "100m",        // CLIENT_MAX_BODY_SIZE
    "html",        // ROOT
    "off",         // AUTOINDEX
    "index.html",  // INDEX
};

Location::Location() : root_(DEFAULTS[ROOT]) {
  setBodyLimit(DEFAULTS[CLIENT_MAX_BODY_SIZE]);
  addAllowedMethod(METHODS[GET]);
  addAllowedMethod(METHODS[POST]);
  setAutoindex(DEFAULTS[AUTOINDEX]);
  addIndex(DEFAULTS[INDEX]);
}

Location::Location(const Location& origin)
    : uri_(origin.uri_),
      body_limit_(origin.body_limit_),
      allowed_methods_(origin.allowed_methods_),
      return_url_(origin.return_url_),
      root_(origin.root_),
      autoindex_(origin.autoindex_),
      index_(origin.index_),
      cgi_param_(origin.cgi_param_),
      is_cgi_(origin.is_cgi_) {}

Location& Location::operator=(const Location& origin) {
  if (this != &origin) {
    uri_ = origin.uri_;
    body_limit_ = origin.body_limit_;
    allowed_methods_ = origin.allowed_methods_;
    return_url_ = origin.return_url_;
    root_ = origin.root_;
    autoindex_ = origin.autoindex_;
    index_ = origin.index_;
    cgi_param_ = origin.cgi_param_;
    is_cgi_ = origin.is_cgi_;
  }
  return *this;
}

Location::~Location() {}

bool Location::isImplementedMethod(const std::string& method) {
  for (std::size_t i = 0; i < METHODS_COUNT; ++i) {
    if (method == METHODS[i]) {
      return true;
    }
  }
  return false;
}

const std::string& Location::getUri(void) const { return uri_; }

std::size_t Location::getBodyLimit(void) const { return body_limit_; }

std::set<std::string>& Location::getAllowedMethods(void) {
  return allowed_methods_;
}

const std::set<std::string>& Location::getAllowedMethods(void) const {
  return allowed_methods_;
}

const std::string& Location::getReturnUrl(void) const { return return_url_; }

const std::string& Location::getRoot(void) const { return root_; }

bool Location::getAutoindex(void) const { return autoindex_; }

std::vector<std::string>& Location::getIndex(void) { return index_; }

const std::vector<std::string>& Location::getIndex(void) const {
  return index_;
}

std::string Location::getCgiParam(const std::string& key) const {
  if (cgi_param_.find(key) == cgi_param_.end()) {
    return "";
  }
  return cgi_param_.at(key);
}

void Location::setUri(const std::string& uri) { uri_ = uri; }

void Location::setBodyLimit(const std::string& raw) {
  static const ByteUnit UNITS;
  char* unit;
  body_limit_ = std::strtoul(raw.c_str(), &unit, 10);
  if (errno == ERANGE) {
    Error::log(Error::INFO[ETOKEN], std::strerror(errno), EXIT_FAILURE);
  }
  if (*unit == '\0') return;
  if (UNITS.size.find(unit) == UNITS.size.end()) {
    Error::log(Error::INFO[ETOKEN], unit, EXIT_FAILURE);
  }
  body_limit_ *= UNITS.size.at(unit);
}

void Location::addAllowedMethod(const std::string& method) {
  allowed_methods_.insert(method);
}

void Location::setReturnUrl(const std::string& return_url) {
  return_url_ = return_url;
}

void Location::setRoot(const std::string& root) { root_ = root; }

void Location::setAutoindex(const std::string& raw) {
  if (raw != "on" && raw != "off") {
    Error::log(Error::INFO[ETOKEN], raw, EXIT_FAILURE);
  }
  autoindex_ = (raw == "on");
}

void Location::addIndex(const std::string& index) { index_.push_back(index); }

void Location::addCgiParam(const std::string& key, const std::string& value) {
  cgi_param_[key] = value;
  is_cgi_ = true;
}

bool Location::isAllowedMethod(const std::string& method) const {
  return allowed_methods_.find(method) != allowed_methods_.end();
}

bool Location::isCgi(void) { return is_cgi_; }

void Location::clear(void) { *this = Location(); }

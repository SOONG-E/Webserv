#include "LocationBlock.hpp"

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

#include "ByteUnits.hpp"
#include "Error.hpp"
#include "constant.hpp"
#include "exception.hpp"

const std::string LocationBlock::DEFAULTS[] = {
    "100m",        // CLIENT_MAX_BODY_SIZE
    "html",        // ROOT
    "off",         // AUTOINDEX
    "index.html",  // INDEX
};

LocationBlock::LocationBlock() : root_(DEFAULTS[ROOT]) {
  setBodyLimit(DEFAULTS[CLIENT_MAX_BODY_SIZE]);
  addAllowedMethod(METHODS[GET]);
  addAllowedMethod(METHODS[POST]);
  setAutoindex(DEFAULTS[AUTOINDEX]);
  addIndex(DEFAULTS[INDEX]);
}

LocationBlock::LocationBlock(const LocationBlock& origin)
    : uri_(origin.uri_),
      body_limit_(origin.body_limit_),
      allowed_methods_(origin.allowed_methods_),
      return_url_(origin.return_url_),
      root_(origin.root_),
      autoindex_(origin.autoindex_),
      index_(origin.index_),
      cgi_param_(origin.cgi_param_) {}

LocationBlock& LocationBlock::operator=(const LocationBlock& origin) {
  if (this != &origin) {
    uri_ = origin.uri_;
    body_limit_ = origin.body_limit_;
    allowed_methods_ = origin.allowed_methods_;
    return_url_ = origin.return_url_;
    root_ = origin.root_;
    autoindex_ = origin.autoindex_;
    index_ = origin.index_;
    cgi_param_ = origin.cgi_param_;
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

const std::set<std::string>& LocationBlock::getAllowedMethods(void) const {
  return allowed_methods_;
}

const std::string& LocationBlock::getReturnUrl(void) const {
  return return_url_;
}

const std::string& LocationBlock::getRoot(void) const { return root_; }

bool LocationBlock::getAutoindex(void) const { return autoindex_; }

std::vector<std::string>& LocationBlock::getIndex(void) { return index_; }

const std::vector<std::string>& LocationBlock::getIndex(void) const {
  return index_;
}

std::string LocationBlock::getCgiParam(const std::string& key) const {
  if (cgi_param_.find(key) == cgi_param_.end()) {
    return "";
  }
  return cgi_param_.at(key);
}

void LocationBlock::setUri(const std::string& uri) { uri_ = uri; }

void LocationBlock::setBodyLimit(const std::string& raw) {
  static const ByteUnits UNITS;
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

void LocationBlock::addAllowedMethod(const std::string& method) {
  allowed_methods_.insert(method);
}

void LocationBlock::setReturnUrl(const std::string& return_url) {
  return_url_ = return_url;
}

void LocationBlock::setRoot(const std::string& root) { root_ = root; }

void LocationBlock::setAutoindex(const std::string& raw) {
  if (raw != "on" && raw != "off") {
    Error::log(Error::INFO[ETOKEN], raw, EXIT_FAILURE);
  }
  autoindex_ = (raw == "on");
}

void LocationBlock::addIndex(const std::string& index) {
  index_.push_back(index);
}

void LocationBlock::addCgiParam(const std::string& key,
                                const std::string& value) {
  cgi_param_[key] = value;
}

bool LocationBlock::isAllowedMethod(const std::string& method) const {
  return allowed_methods_.find(method) != allowed_methods_.end();
}

bool LocationBlock::isCgi(const std::string& request_uri,
                          const std::string& request_method,
                          const std::string& request_query) const {
  if (cgi_param_.find("CGI_EXTENSION") == cgi_param_.end()) {
    return false;
  }
  const std::string& extension = cgi_param_.at("CGI_EXTENSION").substr(1);
  std::size_t index = request_uri.rfind(extension);
  if (index == std::string::npos ||
      index != request_uri.size() - extension.size()) {
    return false;
  }
  if (request_method == METHODS[POST] || !request_query.empty()) {
    return true;
  }
  return false;
}

void LocationBlock::clear(void) { *this = LocationBlock(); }

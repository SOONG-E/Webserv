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
    : root(DEFAULTS[ROOT]), autoindex(DEFAULTS[AUTOINDEX]) {
  setBodyLimit(DEFAULTS[CLIENT_MAX_BODY_SIZE]);
  index.insert(DEFAULTS[INDEX]);
}

void LocationBlock::setBodyLimit(const std::string& raw) {
  static const ByteUnits UNITS;
  char* unit;
  body_limit = std::strtoul(raw.c_str(), &unit, 10);
  if (errno == ERANGE) {
    throw ConfigException(std::strerror(errno));
  }
  if (*unit == '\0') return;
  try {
    body_limit *= UNITS.size.at(unit);
  } catch (std::out_of_range& e) {
    throw ConfigException(ERRORS[TOKEN]);
  }
}

bool LocationBlock::isAllowedMethod(const std::string& method) const {
  return allowed_methods.find(method) != allowed_methods.end();
}

bool LocationBlock::isImplementedMethod(const std::string& method) const {
  for (std::size_t i = 0; i < METHODS_COUNT; ++i) {
    if (method == METHODS[i]) {
      return true;
    }
  }
  return false;
}

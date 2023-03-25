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
  static ByteUnits units;
  char* unit;
  body_limit = std::strtoul(raw.c_str(), &unit, 10);
  if (errno == ERANGE) {
    throw ConfigException(std::strerror(errno));
  }
  if (*unit == '\0') return;
  try {
    body_limit *= units.size.at(unit);
  } catch (const std::out_of_range& e) {
    throw ConfigException(ERRORS[TOKEN]);
  }
}

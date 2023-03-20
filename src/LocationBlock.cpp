#include "LocationBlock.hpp"

#include "constant.hpp"

const std::string LocationBlock::DEFAULTS[] = {"html", "off", "index.html"};

LocationBlock::LocationBlock()
    : root(DEFAULTS[ROOT]), autoindex(DEFAULTS[AUTOINDEX]) {
  for (int i = 0; i < METHODS_COUNT; ++i) {
    allowed_methods.insert(METHODS[i]);
  }
  index.insert(DEFAULTS[INDEX]);
}

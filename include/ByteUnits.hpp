#ifndef BTYE_UNITS_HPP_
#define BYTE_UNITS_HPP_

#include <map>
#include <string>

struct ByteUnits {
  enum Index {
    kLowerKilo,
    kUpperKilo,
    kLowerMega,
    kUpperMega,
    kLowerGiga,
    kUpperGiga,
  };

  static const std::string kKeys[];
  static const std::size_t kValues[];

  ByteUnits() {
    size.insert(std::make_pair(kKeys[kLowerKilo], kValues[kLowerKilo]));
    size.insert(std::make_pair(kKeys[kUpperKilo], kValues[kUpperKilo]));
    size.insert(std::make_pair(kKeys[kLowerMega], kValues[kLowerMega]));
    size.insert(std::make_pair(kKeys[kUpperMega], kValues[kUpperMega]));
    size.insert(std::make_pair(kKeys[kLowerGiga], kValues[kLowerGiga]));
    size.insert(std::make_pair(kKeys[kUpperGiga], kValues[kUpperGiga]));
  }

  std::map<std::string, std::size_t> size;
};

const std::string ByteUnits::kKeys[] = {"k", "K", "m", "M", "g", "G"};
const std::size_t ByteUnits::kValues[] = {1 << 10, 1 << 10, 1 << 20,
                                          1 << 20, 1 << 30, 1 << 30};

#endif

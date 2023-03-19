#ifndef BTYE_UNITS_HPP_
#define BYTE_UNITS_HPP_

#include <map>
#include <string>

struct ByteUnits {
  enum Index {
    LOWER_KILO,
    UPPER_KILO,
    LOWER_MEGA,
    UPPER_MEGA,
    LOWER_GIGA,
    UPPER_GIGA,
  };

  static const std::string KEYS[];
  static const std::size_t VALUES[];

  ByteUnits() {
    size.insert(std::make_pair(KEYS[LOWER_KILO], VALUES[LOWER_KILO]));
    size.insert(std::make_pair(KEYS[UPPER_KILO], VALUES[UPPER_KILO]));
    size.insert(std::make_pair(KEYS[LOWER_MEGA], VALUES[LOWER_MEGA]));
    size.insert(std::make_pair(KEYS[UPPER_MEGA], VALUES[UPPER_MEGA]));
    size.insert(std::make_pair(KEYS[LOWER_GIGA], VALUES[LOWER_GIGA]));
    size.insert(std::make_pair(KEYS[UPPER_GIGA], VALUES[UPPER_GIGA]));
  }

  std::map<std::string, std::size_t> size;
};

const std::string ByteUnits::KEYS[] = {"k", "K", "m", "M", "g", "G"};
const std::size_t ByteUnits::VALUES[] = {1 << 10, 1 << 10, 1 << 20,
                                         1 << 20, 1 << 30, 1 << 30};

#endif

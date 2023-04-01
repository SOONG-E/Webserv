#ifndef LOG_HPP_
#define LOG_HPP_

#include <string>

struct Log {
  enum ColorIndex {
    DEFAULT,
    RED,
    GREEN,
  };

  static const int WIDTH;
  static const char PADDING_CHAR;
  static const std::string FOOTER_SUFFIX;
  static const std::string COLORS[];

  static void pad(const std::string& msg,
                  const std::string& color = COLORS[DEFAULT]);
  static void header(const std::string& msg);
  static void footer(const std::string& msg = "");
};

#endif

#include "Log.hpp"

#include <iomanip>
#include <iostream>

const int Log::WIDTH = 55;
const char Log::PADDING_CHAR = '-';
const std::string Log::FOOTER_SUFFIX = " Success";
const std::string Log::COLORS[] = {
    "\033[0m",     // DEFAULT
    "\033[0;31m",  // RED
    "\033[0;32m",  // GREEN
};

void Log::pad(const std::string& msg, const std::string& color) {
  int left_padding = (WIDTH + msg.size()) / 2;
  int right_padding = WIDTH - left_padding;
  std::cout << color << std::setfill(PADDING_CHAR) << std::setw(left_padding)
            << msg << std::setw(right_padding) << "\n";
}

void Log::header(const std::string& msg) { pad(msg); }

void Log::footer(const std::string& msg) {
  if (msg.empty()) {
    pad(msg);
  } else {
    pad(msg + FOOTER_SUFFIX, COLORS[GREEN]);
  }
  std::cout << COLORS[DEFAULT] << "\n";
}

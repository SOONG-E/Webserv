#include "Log.hpp"

#include <iomanip>
#include <iostream>

void Log::header(const std::string& title) {
  int left_margin = (WIDTH + title.size()) / 2;
  int right_margin = WIDTH - left_margin;
  std::cout << std::setfill(MARGIN_CHAR) << std::setw(left_margin) << title
            << std::setfill(MARGIN_CHAR) << std::setw(right_margin) << "\n";
}

void Log::footer(void) {
  std::cout << std::setfill(MARGIN_CHAR) << std::setw(WIDTH) << "\n\n";
}

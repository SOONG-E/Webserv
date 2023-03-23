#ifndef LOG_HPP_
#define LOG_HPP_

#include <string>

struct Log {
	static const int WIDTH = 55;
	static const char MARGIN_CHAR = '-';

	static void header(const std::string& title);
	static void footer(void);
};

#endif

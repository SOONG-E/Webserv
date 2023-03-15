#ifndef UTILITY_HPP_
#define UTILITY_HPP_

#include <string>
#include <vector>

std::string readFile(const char* filename);
std::vector<std::string> split(const std::string& content,
                               const std::string& separators);
int stoi(const std::string& str);

#endif

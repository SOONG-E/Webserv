#ifndef CONFIG_PARSER_HPP_
#define CONFIG_PARSER_HPP_

#include <fstream>
#include <string>

#include "Config.hpp"
#include "Server.hpp"

class ConfigParser {
 public:
  static Config parse(const std::ifstream& infile);

 private:
  std::string readFile(const std::ifstream& infile);
  Server parseServer(str::string content);
};

#endif

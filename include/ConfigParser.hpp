#ifndef CONFIG_PARSER_HPP_
#define CONFIG_PARSER_HPP_

#include "Config.hpp"

class ConfigParser {
 public:
  ConfigParser();
  explicit ConfigParser(const char* filename);
  ConfigParser(const ConfigParser& origin);
  ConfigParser& operator=(const ConfigParser& origin);

  virtual ~ConfigParser();

  Config parse(void);

 private:
  std::string readFile(const char* filename);
  int stoi(const std::string& str);
  void parseServerBlock(void);
  void parseListen(void);
  void parseServerName(void);
  void parseErrorPage(void);
  void parseClientMaxBodySize(void);
  void parseLocationBlock(void);
  std::string expect(const std::string& expected);
  std::string peek(void);
  void skipWhitespace(void);

  Config config_;
  Server buffer_;
  std::string content_;
  std::size_t pos_;
};

#endif

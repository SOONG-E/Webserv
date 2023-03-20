#ifndef CONFIG_PARSER_HPP_
#define CONFIG_PARSER_HPP_

#include "Config.hpp"

class ConfigParser {
 public:
  explicit ConfigParser(const char* filename);
  ConfigParser(const ConfigParser& origin);
  ConfigParser& operator=(const ConfigParser& origin);
  virtual ~ConfigParser();

  const Config& parse(void);
  void print(void) const;

 private:
  void parseServerBlock(void);
  void parseListen(void);
  void parseServerName(void);
  void parseErrorPage(void);
  void parseClientMaxBodySize(void);

  void parseLocationBlock(void);
  void parseAllowedMethods(void);
  void parseReturn(void);
  void parseRoot(void);
  void parseAutoindex(void);
  void parseIndex(void);

  std::string expect(const std::string& expected = "");
  std::string peek(void);
  void skipWhitespace(void);

  Config config_;
  ServerBlock server_block_;
  LocationBlock location_block_;
  std::string content_;
  std::size_t pos_;
};

#endif

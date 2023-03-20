#include "ConfigParser.hpp"

#include <fstream>
#include <sstream>

#include "constant.hpp"
#include "exception.hpp"
#include "utility.hpp"

ConfigParser::ConfigParser(const char* filename) : pos_(0) {
  content_ = readFile(filename);
}

ConfigParser::ConfigParser(const ConfigParser& origin)
    : config_(origin.config_),
      server_block_(origin.server_block_),
      location_block_(origin.location_block_),
      content_(origin.content_),
      pos_(origin.pos_) {}

ConfigParser& ConfigParser::operator=(const ConfigParser& origin) {
  if (this != &origin) {
    config_ = origin.config_;
    server_block_ = origin.server_block_;
    location_block_ = origin.location_block_;
    content_ = origin.content_;
    pos_ = origin.pos_;
  }
  return *this;
}

ConfigParser::~ConfigParser() {}

const Config& ConfigParser::parse(void) {
  while (peek() == "server") {
    server_block_.reset();
    parseServerBlock();
    config_.addServerBlock(server_block_);
  }
  if (!expect().empty()) {
    throw ConfigException(ERRORS[TOKEN]);
  }
  return config_;
}

void ConfigParser::print(void) const {
  for (std::size_t i = 0; i < config_.getServerBlocks().size(); ++i) {
    config_.getServerBlocks()[i].print(i + 1);
  }
}

void ConfigParser::parseServerBlock(void) {
  expect("server");
  expect("{");
  while (true) {
    std::string token = peek();
    if (token == "}") break;
    if (token == "listen") {
      parseListen();
    } else if (token == "server_name") {
      parseServerName();
    } else if (token == "error_page") {
      parseErrorPage();
    } else if (token == "client_max_body_size") {
      parseClientMaxBodySize();
    } else if (token == "location") {
      parseLocationBlock();
      server_block_.addLocationBlock(location_block_);
    } else {
      throw ConfigException(ERRORS[TOKEN]);
    }
  }
  expect("}");
}

void ConfigParser::parseListen(void) {
  expect("listen");
  server_block_.addListen(expect());
  expect(";");
}

void ConfigParser::parseServerName(void) {
  expect("server_name");
  while (peek() != ";") {
    server_block_.addServerName(expect());
  }
  expect(";");
}

void ConfigParser::parseErrorPage(void) {
  expect("error_page");
  std::vector<std::string> codes;
  while (peek() != ";") {
    codes.push_back(expect());
  }
  std::string page = codes.back();
  codes.pop_back();
  for (std::size_t i = 0; i < codes.size(); ++i) {
    server_block_.addErrorPage(codes[i], page);
  }
  expect(";");
}

void ConfigParser::parseClientMaxBodySize(void) {
  expect("client_max_body_size");
  server_block_.setBodyLimit(expect());
  expect(";");
}

void ConfigParser::parseLocationBlock(void) {
  expect("location");
  location_block_.uri = expect();
  expect("{");
  while (true) {
    std::string token = peek();
    if (token == "}") break;
    if (token == "allowed_methods") {
      parseAllowedMethods();
    } else if (token == "return") {
      parseReturn();
    } else if (token == "root") {
      parseRoot();
    } else if (token == "autoindex") {
      parseAutoindex();
    } else if (token == "index") {
      parseIndex();
    } else {
      throw ConfigException(ERRORS[TOKEN]);
    }
  }
  expect("}");
}

void ConfigParser::parseAllowedMethods(void) {
  expect("allowed_methods");
  while (peek() != ";") {
    location_block_.allowed_methods.insert(expect());
  }
  expect(";");
}

void ConfigParser::parseReturn(void) {
  expect("return");
  location_block_.return_code = expect();
  location_block_.return_url = expect();
  expect(";");
}

void ConfigParser::parseRoot(void) {
  expect("root");
  location_block_.root = expect();
  expect(";");
}

void ConfigParser::parseAutoindex(void) {
  expect("autoindex");
  location_block_.autoindex = expect();
  expect(";");
}

void ConfigParser::parseIndex(void) {
  expect("index");
  while (peek() != ";") {
    location_block_.index.insert(expect());
  }
  expect(";");
}

std::string ConfigParser::expect(const std::string& expected) {
  skipWhitespace();
  if (!expected.empty()) {
    if (content_.substr(pos_, expected.size()) != expected) {
      throw ConfigException(ERRORS[TOKEN]);
    }
    pos_ += expected.size();
    return expected;
  }
  std::string token = "";
  std::string delim = ";{}";
  while (pos_ < content_.size() && !std::isspace(content_[pos_])) {
    if (!token.empty() && delim.find(content_[pos_]) != std::string::npos)
      break;
    token += content_[pos_];
    pos_ += 1;
  }
  return token;
}

std::string ConfigParser::peek(void) {
  std::size_t old_pos = pos_;
  std::string token = expect();
  pos_ = old_pos;
  return token;
}

void ConfigParser::skipWhitespace(void) {
  while (pos_ < content_.size() && std::isspace(content_[pos_])) {
    pos_ += 1;
  }
}

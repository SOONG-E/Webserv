#include "ConfigParser.hpp"

#include <cstdlib>
#include <fstream>
#include <sstream>
#include <vector>

#include "Error.hpp"
#include "exception.hpp"
#include "utility.hpp"

ConfigParser::ConfigParser(const std::string& filename) : pos_(0) {
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
  int index = 0;
  while (peek() == "server") {
    server_block_.clear();
    parseServerBlock();
    server_block_.setKey(index);
    config_.addServerBlock(server_block_);
    index += 1;
  }
  std::string token = expect();
  if (!token.empty()) {
    Error::log(Error::INFO[ETOKEN], token, EXIT_FAILURE);
  }
  return config_;
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
    } else if (token == "location") {
      location_block_.clear();
      parseLocationBlock();
      server_block_.addLocationBlock(location_block_);
    } else {
      Error::log(Error::INFO[ETOKEN], token, EXIT_FAILURE);
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
  const std::string& page = codes.back();
  codes.pop_back();
  for (std::size_t i = 0; i < codes.size(); ++i) {
    server_block_.addErrorPage(codes[i], page);
  }
  expect(";");
}

void ConfigParser::parseClientMaxBodySize(void) {
  expect("client_max_body_size");
  location_block_.setBodyLimit(expect());
  expect(";");
}

void ConfigParser::parseLocationBlock(void) {
  expect("location");
  location_block_.setUri(expect());
  expect("{");
  while (true) {
    std::string token = peek();
    if (token == "}") break;
    if (token == "client_max_body_size") {
      parseClientMaxBodySize();
    } else if (token == "allowed_methods") {
      parseAllowedMethods();
    } else if (token == "return") {
      parseReturn();
    } else if (token == "root") {
      parseRoot();
    } else if (token == "autoindex") {
      parseAutoindex();
    } else if (token == "index") {
      parseIndex();
    } else if (token.compare(0, 4, "CGI_") == 0) {
      parseCgiParams();
    } else {
      Error::log(Error::INFO[ETOKEN], token, EXIT_FAILURE);
    }
  }
  expect("}");
}

void ConfigParser::parseAllowedMethods(void) {
  expect("allowed_methods");
  location_block_.getAllowedMethods().clear();
  while (peek() != ";") {
    location_block_.addAllowedMethod(expect());
  }
  expect(";");
}

void ConfigParser::parseReturn(void) {
  expect("return");
  location_block_.setReturnUrl(expect());
  expect(";");
}

void ConfigParser::parseRoot(void) {
  expect("root");
  location_block_.setRoot(expect());
  expect(";");
}

void ConfigParser::parseAutoindex(void) {
  expect("autoindex");
  location_block_.setAutoindex(expect());
  expect(";");
}

void ConfigParser::parseIndex(void) {
  expect("index");
  location_block_.getIndex().clear();
  while (peek() != ";") {
    location_block_.addIndex(expect());
  }
  expect(";");
}

void ConfigParser::parseCgiParams(void) {
  std::string key = expect();
  std::string value = expect();
  location_block_.addCgiParam(key, value);
  expect(";");
}

std::string ConfigParser::expect(const std::string& expected) {
  skipWhitespace();
  std::string token;
  if (!expected.empty()) {
    token = content_.substr(pos_, expected.size());
    if (token != expected) {
      Error::log(Error::INFO[ETOKEN], token, EXIT_FAILURE);
    }
    pos_ += expected.size();
    return expected;
  }
  const std::string delim = ";{}";
  token = "";
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

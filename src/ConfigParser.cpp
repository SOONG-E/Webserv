#include "ConfigParser.hpp"

#include <fstream>
#include <sstream>

ConfigParser::ConfigParser() {}

ConfigParser::ConfigParser(const char* filename) {
  content_ = readFile(filename);
}

ConfigParser::ConfigParser(const ConfigParser& origin) {}

ConfigParser& ConfigParser::operator=(const ConfigParser& origin) {}

ConfigParser::~ConfigParser() {}

Config ConfigParser::parse(void) {
  while (peek() == "server") {
    parseServerBlock();
    config_.addServer(buffer_);
  }
  return config_;
}

void ConfigParser::parseServerBlock(void) {
  expect("server");
  expect("{");
  while (peek() != "}") {
    if (peek() == "listen") {
      parseListen();
    } else if (peek() == "server_name") {
      parseServerName();
    } else if (peek() == "error_page") {
      parseErrorPage();
    } else if (peek() == "client_max_body_size") {
      parseClientMaxBodySize();
      // } else if (peek() == "location") {
      //   parseLocationBlock();
    } else {
      throw ConfigException(kErrors[kToken]);
    }
  }
  expect("}");
}

void ConfigParser::parseListen(void) {
  expect("listen");
  std::vector<std::string> listen = split(expect(), ":");
  if (listen.size() == 1) {
    buffer_.addListen("0.0.0.0", stoi(listen[0]));
  } else if (listen.size() == 2) {
    buffer_.addListen(listen[0], stoi(listen[1]));
  } else {
    throw ConfigException(kErrors[kToken]);
  }
  expect(";");
}

void ConfigParser::parseServerName(void) {
  expect("server_name");
  while (peek() != ";") {
    buffer_.addServerName(expect());
  }
  expect(";");
}

void ConfigParser::parseErrorPage(void) {
  expect("error_page");
  int code = stoi(expect());
  std::string page = expect();
  buffer_.addErrorPage(code, page);
  expect(";");
}

void ConfigParser::parseClientBodyLimit(void) {
  expect("client_max_body_size");
  buffer_.body_limit_ = stoi(expect());
  expect(";");
}

void ConfigParser::parseLocationBlock(void) { expect("location"); }

std::string ConfigParser::expect(const std::string& expected = "") {
  skipWhitespace();
  if (expected != "") {
    if (content_.substr(pos_, expected.size()) != expected) {
      throw ConfigException(kErrors[kToken]);
    }
    pos_ += expected.size();
    return expected;
  }
  std::string token;
  while (pos_ < content_.size() && !std::isspace(content_[pos_]) &&
         content_[pos_] != ';' && content_[pos_] != '{' &&
         content_[pos_] != '}') {
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

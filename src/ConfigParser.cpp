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
    buffer_.init();
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
  buffer_.addListen(expect());
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
  std::vector<std::string> codes;
  while (peek() != ";") {
    codes.push_back(expect());
  }
  std::string page = codes.back();
  codes.pop_back();
  std::vector<std::string>::iterator codes_iter = codes.begin();
  std::vector<std::string>::iterator codes_end = codes.end();
  while (begin != end) {
    buffer_.addErrorPage(stoi(*codes_iter), page);
    ++codes_iter;
  }
  expect(";");
}

void ConfigParser::parseClientBodyLimit(void) {
  expect("client_max_body_size");
  buffer_.setBodyLimit(expect());
  expect(";");
}

void ConfigParser::parseLocationBlock(void) {
  expect("location");
  expect();
  expect("{");
  expect("try_files");
  expect();
  expect("}");
}

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

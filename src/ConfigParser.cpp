#include "ConfigParser.hpp"

#include <fstream>
#include <sstream>

#include "ConfigException.hpp"
#include "FileOpenException.hpp"
#include "constant.hpp"

std::string readFile(const char* filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    throw FileOpenException();
  }
  std::ostringstream content;
  content << file.rdbuf();
  file.close();
  return content.str();
}

Config ConfigParser::parse(const char* filename) {
  content_ = readFile(filename);
  while (peek() == "server") {
    parseServerBlock();
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
    } else if (peek() == "location") {
      parseLocationBlock();
    } else {
      throw ConfigException(kErrors[kToken]);
    }
  }
  expect("}");
}

void ConfigParser::parseListen(void) {
  expect("listen");
  listen_ = expect();
  expect(";");
}

void ConfigParser::parseServerName(void) {
  expect("server_name");
  while (peek() != ";") {
    server_names_.insert(expect());
  }
  expect(";");
}

void ConfigParser::parseErrorPage(void) {
  expect("error_page");
  std::string code = expect();
  std::string page = expect();
  error_pages_.insert(std::make_pair(code, page));
  expect(";");
}

void ConfigParser::parseClientBodyLimit(void) {
  expect("client_max_body_size");
  body_limit_ = expect();
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

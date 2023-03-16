#include "ConfigParser.hpp"

#include <fstream>
#include <sstream>

#include "constant.hpp"
#include "exception.hpp"
#include "utility.hpp"

ConfigParser::ConfigParser() {}

ConfigParser::ConfigParser(const char* filename) {
  content_ = readFile(filename);
}

ConfigParser::ConfigParser(const ConfigParser& origin) {}

ConfigParser& ConfigParser::operator=(const ConfigParser& origin) {
  return *this;
}

ConfigParser::~ConfigParser() {}

Config ConfigParser::parse(void) {
  while (peek() == "server") {
    server_ = new Server;
    parseServerBlock();
    config_.addServer(server_);
  }
  if (expect() != "") {
    throw ConfigException(kErrors[kToken]);
  }
  return config_;
}

void ConfigParser::print(void) {
  for (int i = 0; i < config_.getServers().size(); ++i) {
    config_.getServers()[i]->print(i);
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
      location_ = new Location;
      parseLocationBlock();
      server_->addLocation(location_);
    } else {
      throw ConfigException(kErrors[kToken]);
    }
  }
  expect("}");
}

void ConfigParser::parseListen(void) {
  expect("listen");
  server_->addListen(expect());
  expect(";");
}

void ConfigParser::parseServerName(void) {
  expect("server_name");
  while (peek() != ";") {
    server_->addServerName(expect());
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
    server_->addErrorPage(codes[i], page);
  }
  expect(";");
}

void ConfigParser::parseClientMaxBodySize(void) {
  expect("client_max_body_size");
  server_->setBodyLimit(expect());
  expect(";");
}

void ConfigParser::parseLocationBlock(void) {
  expect("location");
  location_->uri = expect();
  expect("{");
  while (true) {
    std::string token = peek();
    if (token == "}") break;
    if (token == "limit_except") {
      parseLimitExcept();
    }
  }
  expect("}");
}

void ConfigParser::parseLimitExcept(void) {
  expect("limit_except");
  while (peek() != "{") {
    location_->allowed_methods.insert(expect());
  }
  expect("{");
  expect("deny");
  expect("all");
  expect(";");
  expect("}");
}

std::string ConfigParser::expect(const std::string& expected) {
  skipWhitespace();
  if (expected != "") {
    if (content_.substr(pos_, expected.size()) != expected) {
      throw ConfigException(kErrors[kToken]);
    }
    pos_ += expected.size();
    return expected;
  }
  std::string token = "";
  std::string delim = ";{}";
  while (pos_ < content_.size() && !std::isspace(content_[pos_])) {
    if (token != "" && delim.find(content_[pos_]) != std::string::npos) break;
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

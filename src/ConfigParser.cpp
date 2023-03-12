#include "ConfigParser.hpp"

#include <queue>
#include <sstream>

#include "ConfigException.hpp"
#include "FileOpenException.hpp"

std::string readFile(const std::ifstream& file) {
  std::stringstream content;
  content << file.rdbuf();
  return content.str();
}

std::queue<std::string> split(const std::string& content,
                              const std::string& separators = " \t\n\v\f\r") {
  std::queue<std::string> substrings;
  std::size_t start = 0;
  std::size_t end;
  while (true) {
    end = content.find_first_of(separators);
    if (end == std::string::npos) break;
    substrings.push(content.substr(start, end - start));
    start = end + 1;
  }
  substrings.push(content.substr(start));
  return substrings;
}

Server ConfigParser::parseServer(str::string content) {
  Server server;
  std::queue<std::string> lines;
  lines = split(content, ";");
  while (!lines.empty()) {
  }
}

Config ConfigParser::parse(const std::ifstream& infile) {
  if (!infile.is_open()) throw FileOpenException();
  std::string content = readFile(infile);
  std::queue<std::string> substrings = split(content, "{}");
  Config config;
  while (!substrings.empty()) {
    Server server = parseServer(substrings.front());
    std::set<std::string> names = server.getServerNames();
    std::set<std::string>::iterator iter = names.begin();
    while (iter != names.end()) {
      config.insertServer(*iter, server);
      iter++;
    }
    substrings.pop();
  }
  return config;
}

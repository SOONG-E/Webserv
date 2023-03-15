#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "./include/HttpRequest.hpp"
#include "./include/HttpRequestParser.hpp"

int main() {
  std::ifstream file("./request.txt");
  std::ostringstream content;
  content << file.rdbuf();
  file.close();
  std::string request(content.str());

  HttpRequest hr = HttpRequestParser::parse(request);

  std::cout << hr.getHost() << std::endl;
}
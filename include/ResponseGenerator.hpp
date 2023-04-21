#ifndef RESPONSE_GENERATOR_HPP_
#define RESPONSE_GENERATOR_HPP_

#include "Client.hpp"
#include "FileOpenException.hpp"
#include "HttpRequest.hpp"
#include "Response.hpp"
#include "utility.hpp"

class Client;

class ResponseGenerator {
 public:
  static std::string &generateResponse(Client &client,
                                       struct Response &response_dummy);

 private:
  static void generateStatusLine(std::string &response, Client &client);
  static void generateHeader(std::string &response, Client &client,
                             const std::map<std::string, std::string> &headers);

  static void generateGeneralHeader(std::string &response, Client &client);
  static void generateEntityHeader(std::string &response, Client &client);
  static std::string generateCookie(HttpRequest &request);
  static std::string getConnectionHeader(Client &client);
  static std::string getDateHeader(void);
};

#endif
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
  static std::string generateResponse(Client &client,
                                      struct Response &response);

 private:
  static std::string generateStatusLine(Client &client);
  static std::string generateHeader(
      Client &client, const std::map<std::string, std::string> &headers);

  static std::string generateGeneralHeader(Client &client);
  static std::string generateEntityHeader(Client &client);
  static std::string generateCookie(HttpRequest &request);
  static std::string getConnectionHeader(Client &client);
  static std::string getDateHeader(void);
};

#endif
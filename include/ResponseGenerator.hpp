#ifndef RESPONSE_GENERATOR_HPP_
#define RESPONSE_GENERATOR_HPP_

#include "FileOpenException.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "utility.hpp"

class ResponseGenerator {
 public:
  static std::string generateResponse(HttpResponse &response,
                                      HttpRequest &request);

 private:
  static std::string generateBody(HttpRequest &request, HttpResponse &response,
                                  std::string &body);
  static std::string generateHeader(HttpRequest &request,
                                    HttpResponse &response, std::string &header,
                                    const std::string &body);
  static std::string generatePage(HttpRequest &request, HttpResponse &response,
                                  std::string &body);
  static std::string generateErrorPage(HttpResponse &response);

  static void processCgiResponse(HttpResponse &response, std::string &header,
                                 std::string &body);
  static bool isCgi(HttpRequest &request, HttpResponse &response);
  static void generateHeaderFromCgi(HttpResponse &response,
                                    std::string &header);

  static std::string generateGeneralHeader(HttpRequest &request,
                                           HttpResponse &response,
                                           std::string &header);
  static std::string generateEntityHeader(HttpRequest &request,
                                          HttpResponse &response,
                                          std::string &header,
                                          const std::string &body);
  static std::string generateCookie(HttpRequest &request);
  static std::string getConnectionHeader(HttpRequest &request,
                                         HttpResponse &response);
  static std::string getDateHeader(void);
  static std::string getTransferEncodingHeader(HttpRequest &request);
  static std::string directoryListing(const std::string &url);
  static std::string readIndexFile(HttpResponse &response,
                                   const std::string &url);
  static std::string readFile(HttpResponse &response, const std::string &uri);
  static void replaceNLtoCRLF(std::string &header);
};

#endif
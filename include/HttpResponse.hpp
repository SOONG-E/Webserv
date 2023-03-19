#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include <string>

#include "HttpRequest.hpp"
#include "ServerBlock.hpp"

class HttpResponse {
 public:
  static const std::string DEFAULT_CODE;

  HttpResponse();
  HttpResponse(const HttpResponse& origin);
  HttpResponse& operator=(const HttpResponse& origin);
  ~HttpResponse();

  const std::string& getCode(void) const;
  void setCode(const std::string& code);
  void setReason(const std::string& reason);

  void generateStatusLine();
  void generateHeader();
  std::string generate(const HttpRequest& request,
                       const ServerBlock* server_block);

 private:
  std::string getDate() const;

  std::string code_;
  std::string reason_;
  std::string header_;
  std::string body_;
};

const std::string HttpResponse::DEFAULT_CODE = "200";

#endif

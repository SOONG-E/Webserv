#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include <string>

#include "HttpRequest.hpp"
#include "ServerBlock.hpp"

class HttpResponse {
 public:
  enum Index {
    CODE,
    ERROR_PAGE,
  };

  static const std::string DEFAULTS[];

  HttpResponse();
  HttpResponse(const HttpResponse& origin);
  HttpResponse& operator=(const HttpResponse& origin);
  ~HttpResponse();

  const std::string& getCode(void) const;
  void setCode(const std::string& code);
  void setReason(const std::string& reason);

  std::string generate(const HttpRequest& request,
                       const ServerBlock* server_block);

 private:
  std::string getDate(void) const;
  void generateHeader(std::string& header);
  std::string rootUri(const std::string& request_uri,
                      const std::vector<LocationBlock>& locations);

  std::string code_;
  std::string reason_;
  std::string backup_;
  std::string header_;
  std::string body_;
};

#endif

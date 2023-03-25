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

  const ServerBlock* getServerBlock(void) const;
  const LocationBlock* getLocationBlock(void) const;

  void setStatus(const std::string& code, const std::string& reason);
  void setServerBlock(const ServerBlock* server_block);
  void setLocationBlock(const LocationBlock* location_block);

  bool isAllowedMethod(std::string method) const;
  bool isSuccess(void) const;
  std::string generate(const HttpRequest& request);

 private:
  std::string generateResponse(const HttpRequest& request,
                               const std::string& body) const;
  std::string currentTime(void) const;
  std::string rootUri(const std::string& request_uri) const;
  std::string readIndexFile(const std::set<std::string>& index,
                            const std::string& filename) const;

  std::string code_;
  std::string reason_;
  const ServerBlock* server_block_;
  const LocationBlock* location_block_;
};

#endif

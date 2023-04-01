#ifndef HTTP_RESPONSE_HPP_
#define HTTP_RESPONSE_HPP_

#include <map>
#include <string>

#include "HttpRequest.hpp"
#include "LocationBlock.hpp"
#include "ServerBlock.hpp"

class HttpResponse {
  static const int DEFAULT_STATUS;

 public:
  static const std::string DEFAULT_ERROR_PAGE;

  explicit HttpResponse(const ServerBlock& default_server);
  HttpResponse(const HttpResponse& origin);
  ~HttpResponse();

  const ServerBlock* getServerBlock(void) const;
  const LocationBlock* getLocationBlock(void) const;

  void setStatus(const int status);
  void setServerBlock(const ServerBlock* server_block);
  void setLocationBlock(const LocationBlock* location_block);

  void clear(void);
  bool isSuccessCode(void) const;
  std::string generate(const HttpRequest& request, bool is_cgi,
                       const std::string& cgi_response);

 private:
  std::string generateErrorPage(const HttpRequest& request);
  std::string generateResponse(const HttpRequest& request,
                               const std::string& body) const;
  std::string generateFromCgi(const HttpRequest& request,
                              std::string cgi_response) const;
  std::string combine(const HttpRequest& request,
                      const std::string& body) const;
  std::string commonHeader(const HttpRequest& request) const;
  std::string rootUri(std::string uri) const;
  std::string readIndexFile(const std::string& url) const;
  std::string directoryListing(const std::string& url) const;

  int status_;
  const ServerBlock& default_server_;
  const ServerBlock* server_block_;
  const LocationBlock* location_block_;
};

#endif

#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include <string>

#include "HttpRequest.hpp"
#include "ResponseStatus.hpp"
#include "ServerBlock.hpp"

class HttpResponse {
  static const int DEFAULT_INDEX = C200;

 public:
  static const std::string DEFAULT_ERROR_PAGE;

  explicit HttpResponse(const ServerBlock& default_server);
  HttpResponse(const HttpResponse& origin);
  ~HttpResponse();

  const ServerBlock* getServerBlock(void) const;
  const LocationBlock* getLocationBlock(void) const;

  void setStatus(const int index);
  void setServerBlock(const ServerBlock* server_block);
  void setLocationBlock(const LocationBlock* location_block);

  void clear(void);
  bool isSuccessCode(void) const;
  std::string generate(HttpRequest& request);

 private:
  std::string generateResponse(const HttpRequest& request,
                               const std::string& body) const;
  std::string combine(const HttpRequest& request,
                      const std::string& body) const;
  std::string currentTime(void) const;
  std::string rootUri(std::string& request_uri) const;
  std::string readIndexFile(const std::string& filename,
                            const std::set<std::string>& index) const;

  std::string code_;
  std::string reason_;
  const ServerBlock& default_server_;
  const ServerBlock* server_block_;
  const LocationBlock* location_block_;
};

#endif

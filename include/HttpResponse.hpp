#ifndef HTTP_RESPONSE_HPP_
#define HTTP_RESPONSE_HPP_

#include <map>
#include <string>

#include "HttpRequest.hpp"
#include "LocationBlock.hpp"
#include "ServerBlock.hpp"
#include "Session.hpp"

class HttpResponse {
  static const int DEFAULT_STATUS;

 public:
  static const std::string DEFAULT_ERROR_PAGE;

  HttpResponse();
  HttpResponse(const HttpResponse& origin);
  ~HttpResponse();

  int getStatus(void) const;
  Session* getSession(void) const;
  const ServerBlock& getServerBlock(void) const;
  const LocationBlock& getLocationBlock(void) const;
  const std::string& getFullUri(void) const;
  const std::string& getCgiResponse(void) const;

  void setStatus(const int status);
  void setSession(Session* session);
  void setServerBlock(const ServerBlock& server_block);
  void setLocationBlock(const LocationBlock& location_block);
  void setFullUri(const std::string& fullUri);
  void setCgiResponse(const std::string& cgiResponse);

  void clear(void);
  bool isSuccessCode(void) const;

  int status_;
  Session* session_;
  ServerBlock server_block_;
  LocationBlock location_block_;
  std::string fullUri_;
  std::string cgiResponse_;
};

#endif

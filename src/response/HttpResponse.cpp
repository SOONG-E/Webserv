#include "HttpResponse.hpp"

#include "utility.hpp"

const int HttpResponse::DEFAULT_STATUS = C200;
const std::string HttpResponse::DEFAULT_ERROR_PAGE = "html/error.html";

HttpResponse::HttpResponse()
    : status_(DEFAULT_STATUS),
      session_(NULL)  // 임시
      {};

HttpResponse::HttpResponse(const HttpResponse& origin)
    : status_(origin.status_), session_(origin.session_) {}

HttpResponse::~HttpResponse() {}

/*==========================*/
//         Getter           //
/*==========================*/

int HttpResponse::getStatus(void) const { return status_; }

Session* HttpResponse::getSession(void) const { return session_; }

const ServerBlock& HttpResponse::getServerBlock(void) const {
  return server_block_;
}

const LocationBlock& HttpResponse::getLocationBlock(void) const {
  return location_block_;
}

const std::string& HttpResponse::getFullUri(void) const { return fullUri_; }

const std::string& HttpResponse::getCgiResponse(void) const {
  return cgiResponse_;
}

/*==========================*/
//         Setter           //
/*==========================*/

void HttpResponse::setStatus(const int status) { status_ = status; }

void HttpResponse::setSession(Session* session) { session_ = session; }

void HttpResponse::setServerBlock(const ServerBlock& server_block) {
  server_block_ = server_block;
}

void HttpResponse::setLocationBlock(const LocationBlock& location_block) {
  location_block_ = location_block;
}

void HttpResponse::setFullUri(const std::string& fullUri) {
  fullUri_ = fullUri;
}

void HttpResponse::setCgiResponse(const std::string& cgiResponse) {
  cgiResponse_ = cgiResponse;
}

/*===============================================================================*/

void HttpResponse::generate(HttpRequest& request) {
  ResponseGenerator::generateResponse(*this, request);
}

void HttpResponse::clear(void) { status_ = DEFAULT_STATUS; }

bool HttpResponse::isSuccessCode(void) const { return status_ <= C204; }

// std::string HttpResponse::generateFromCgi(const HttpRequest& request,
//                                           std::string cgi_response) const {
//   std::string response = commonHeader(request);
//   const std::string STATUS = "Status: ";
//   std::size_t pos = cgi_response.find(STATUS);
//   if (pos != std::string::npos) {
//     std::size_t count = cgi_response.find(CRLF, pos) - pos;
//     std::string status_line = cgi_response.substr(pos, count);
//     cgi_response.erase(pos, count + CRLF.size());
//     status_line.replace(0, STATUS.size(), "HTTP/1.1 ");
//     response.replace(0, response.find(CRLF), status_line);
//   }
//   response += cgi_response;
//   std::size_t bound_pos = response.find(DOUBLE_CRLF);
//   if (bound_pos == std::string::npos) {
//     response += CRLF;
//     if (session_ &&
//         response.find("HTTP/1.1 201 Created") != std::string::npos) {
//       response +=
//           directoryListing("upload_file/" + toString(server_block_->getKey())
//           +
//                            "/" + session_->getID() + "/");

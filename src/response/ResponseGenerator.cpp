#include "ResponseGenerator.hpp"

std::string &ResponseGenerator::generateResponse(
    Client &client, struct Response &response_dummy) {
  std::string &response = client.getResponse();
  generateStatusLine(response, client, response_dummy);
  generateHeader(response, client, response_dummy);
  if (client.getRequest().getMethod() == METHODS[HEAD]) {
    return response;
  }
  response += response_dummy.body;

  return response;
}

void ResponseGenerator::generateStatusLine(std::string &response,
                                           Client &client,
                                           struct Response &response_dummy) {
  int status = client.getStatus();

  response += "HTTP/1.1 ";

  std::map<std::string, std::string>::iterator status_header =
      response_dummy.headers.find("Status");
  if (status_header != response_dummy.headers.end()) {
    response_dummy.headers.erase("Status");
    response += status_header->second;
    return;
  }

  response += ResponseStatus::CODES[status] + " " +
              ResponseStatus::REASONS[status] + CRLF;
}

/*===============================
 generate, combine header field
===============================*/

void ResponseGenerator::generateHeader(std::string &response, Client &client,
                                       struct Response &response_dummy) {
  generateGeneralHeader(response, client);
  generateEntityHeader(response, client, response_dummy);
  // response += generateCookie(request) + CRLF;
  const std::map<std::string, std::string> &headers = response_dummy.headers;
  for (std::map<std::string, std::string>::const_iterator it = headers.begin();
       it != headers.end(); it++) {
    response += it->first + " : ";
    response += it->second + CRLF;
  }
  response += CRLF;
}

void ResponseGenerator::generateGeneralHeader(std::string &response,
                                              Client &client) {
  response += getConnectionHeader(client) + CRLF;
  response += getDateHeader() + CRLF;
  response += "Cache-Control: no-cache, no-store, must-revalidate" + CRLF;
}

void ResponseGenerator::generateEntityHeader(std::string &response,
                                             Client &client,
                                             struct Response &response_dummy) {
  response += "Server: Webserv" + CRLF;
  response +=
      "Allow: " + join(client.getLocation().getAllowedMethods(), ", ") + CRLF;
  response += "Content-Type: text/html" + CRLF;
  response += "Content-Length: " + toString(response_dummy.body.size()) + CRLF;
}

/*============================
  create header field
============================*/

std::string ResponseGenerator::generateCookie(HttpRequest &request) {
  (void)request;
  return "";  // temp
  // if (session_ && request.getCookie("Session-ID") != session_->getID()) {
  //   header += "Set-Cookie: Session-ID=" + session_->getID() +
  //             "; Max-Age=" + COOKIE_MAX_AGE + "; HttpOnly;" + CRLF;
  // }
}

std::string ResponseGenerator::getConnectionHeader(Client &client) {
  const std::string header_name = "Connection";
  if (client.isErrorCode() == true) {
    return (header_name + ": close");
  }
  if (client.getRequest().getHeader("CONNECTION").empty() == false) {
    return (header_name + " : " + client.getRequest().getHeader("CONNECTION"));
  }
  return (header_name + " : " + "keep-alive");
}

std::string ResponseGenerator::getDateHeader(void) {
  const std::string header_name = "Date";
  return (header_name + " : " + formatTime("%a, %d %b %Y %H:%M:%S GMT"));
}

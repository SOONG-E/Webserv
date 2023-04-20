#include "ResponseGenerator.hpp"

std::string ResponseGenerator::generateResponse(
    Client &client, struct Response &response_dummy) {
  std::string response;
  response = generateStatusLine(client, response_dummy);
  response += generateHeader(client, response_dummy.headers);
  response += response_dummy.body;

  return response;
}

std::string ResponseGenerator::generateStatusLine(Client &client,
                                                  struct Response &response) {
  std::string status = client.getStatus();
  std::map<std::string, std::string>::iterator status_iterator =
      response.headers.find("Status");
  if (status_iterator != response.headers.end()) {
    status = status_iterator->second;
    response.headers.erase(status_iterator);
  }
  return "HTTP/1.1 " + status + ResponseStatus::REASONS[client.getStatusInt()] +
         CRLF;
}

/*==========================*/
//  generate header field   //
/*==========================*/

std::string ResponseGenerator::generateHeader(
    Client &client, const std::map<std::string, std::string> &headers) {
  std::string header;
  header += generateGeneralHeader(client);
  header += generateEntityHeader(client);
  // header += generateCookie(request) + CRLF;
  for (std::map<std::string, std::string>::const_iterator it = headers.begin();
       it != headers.end(); it++) {
    header += it->first + " : ";
    header += it->second + CRLF;
  }
  header += CRLF;

  return header;
}

std::string ResponseGenerator::generateGeneralHeader(Client &client) {
  std::string header;
  header += getConnectionHeader(client) + CRLF;
  header += getDateHeader() + CRLF;
  header += "Cache-Control: no-cache, no-store, must-revalidate" + CRLF;

  return header;
}

std::string ResponseGenerator::generateEntityHeader(Client &client) {
  std::string header;
  header += "Server: Webserv" + CRLF;
  header +=
      "Allow: " + join(client.getLocation().getAllowedMethods(), ", ") + CRLF;
  header += "Content-Type: text/html" + CRLF;
  return header;
}

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
    return (header_name + "close");
  }
  if (client.getRequest().getHeader("CONNECTION").empty() == false) {
    return (header_name + client.getRequest().getHeader("CONNECTION"));
  }
  return (header_name + " : " + "keep-alive");
}

std::string ResponseGenerator::getDateHeader(void) {
  const std::string header_name = "Date";
  return (header_name + " : " + formatTime("%a, %d %b %Y %H:%M:%S GMT"));
}

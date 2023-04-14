#include "ResponseGenerator.hpp"

#include <dirent.h>
#include <sys/stat.h>

#include <set>

#include "DirectoryListingHtml.hpp"
#include "File.hpp"

/*====================================================================*/
// generate Http response using request, server block, location block
//
//  1. if request is for cgi, making header, body from cgi response
//  2. generate body unless body already exist
//    2-1. if status code isn't 2xx, load proper error page
//    2-2. load a page that user asked
//  3. generate header
//    3-1. generate status line and general header, entity header
//          then set cookie only if it's needed
//  - set error page if any exception is throwed while processing
/*====================================================================*/

std::string ResponseGenerator::generateResponse(HttpResponse &response,
                                                HttpRequest &request) {
  std::string header;
  std::string body;

  if (isCgi(request, response) == true) {
    processCgiResponse(response, header, body);
  }
  generateBody(request, response, body);
  generateHeader(request, response, header, body);

  if (request.getMethod() == METHODS[HEAD]) {
    return header;
  }
  return header + body;
}

void ResponseGenerator::generateBody(HttpRequest &request,
                                     HttpResponse &response,
                                     std::string &body) {
  if (response.isSuccessCode() == false) {
    body = generateErrorPage(response);
    return;
  }
  if (isCgi(request, response) == true) {
    return;
  }
  generatePage(response, body);
}

void ResponseGenerator::generateHeader(HttpRequest &request,
                                       HttpResponse &response,
                                       std::string &header,
                                       const std::string &body) {
  if (header.empty()) {
    header = "HTTP/1.1 " + ResponseStatus::CODES[response.getStatus()] + " " +
             ResponseStatus::REASONS[response.getStatus()] + CRLF;
  }
  generateGeneralHeader(request, response, header);
  generateEntityHeader(response, header, body);
  // header += generateCookie(request) + CRLF;
  header += CRLF;
}

void ResponseGenerator::generatePage(HttpResponse &response,
                                     std::string &body) {
  std::string uri = response.getFullUri();
  try {
    body = readFile(response, uri);
  } catch (FileOpenException &e) {
    response.setStatus(C404);
    body = readFile(
        response,
        response.getServerBlock().getErrorPage(
            ResponseStatus::CODES[C404]));  // 이렇게 해해야야하하나나????
  } catch (std::exception &e) {
    response.setStatus(C500);
    body = generateErrorPage(response);
  }
}

std::string ResponseGenerator::generateErrorPage(HttpResponse &response) {
  const ServerBlock &server_block = response.getServerBlock();
  std::string body = readFile(
      response,
      server_block.getErrorPage(ResponseStatus::CODES[response.getStatus()]));
  return body;
}

/*==========================*/
//     process for CGI      //
/*==========================*/

void ResponseGenerator::processCgiResponse(HttpResponse &response,
                                           std::string &header,
                                           std::string &body) {
  std::string cgiResponse = response.getCgiResponse();
  const std::string doubleNL = "\n\n";
  std::size_t boundary = cgiResponse.find(doubleNL);

  if (boundary == std::string::npos) {
    header = cgiResponse;
  } else {
    header = cgiResponse.substr(0, boundary);
    body = cgiResponse.substr(boundary + doubleNL.length());
  }
  generateHeaderFromCgi(response, header);
}

bool ResponseGenerator::isCgi(HttpRequest &request, HttpResponse &response) {
  std::string method = request.getMethod();
  if (!(method == METHODS[POST] || method == METHODS[GET])) {
    return false;
  }
  const Location location_block = response.getLocation();
  return location_block.isCgi(request.getUri(), method,
                              request.getQueryString());
}

void ResponseGenerator::generateHeaderFromCgi(HttpResponse &response,
                                              std::string &header) {
  replaceNLtoCRLF(header);

  const std::string STATUS = "Status:";
  std::size_t pos = header.find(STATUS);
  if (pos == std::string::npos) {
    response.setStatus(C500);
  }
  std::size_t line_length = header.find(CRLF, pos) - pos + CRLF.length();
  std::string status_line = header.substr(pos, line_length);
  header.erase(pos, line_length);
  status_line.replace(0, STATUS.length(), "HTTP/1.1 ");
  header.insert(0, status_line);
}

/*==========================*/
//  generate header field   //
/*==========================*/

void ResponseGenerator::generateGeneralHeader(HttpRequest &request,
                                              HttpResponse &response,
                                              std::string &header) {
  header += getConnectionHeader(request, response) + CRLF;
  header += getDateHeader() + CRLF;
  header += getTransferEncodingHeader(request);
  header += "Cache-Control: no-cache, no-store, must-revalidate" + CRLF;
}

void ResponseGenerator::generateEntityHeader(HttpResponse &response,
                                             std::string &header,
                                             const std::string &body) {
  header += "Server: Webserv" + CRLF;
  header +=
      "Allow: " + join(response.getLocation().getAllowedMethods(), ", ") + CRLF;
  if (body.empty() == false) {
    header += "Content_Length: " + toString(body.length()) + CRLF;
    header += "Content-Type: text/html" + CRLF;
  }
}

std::string ResponseGenerator::generateCookie(HttpRequest &request) {
  (void)request;
  return "";  // temp
  // if (session_ && request.getCookie("Session-ID") != session_->getID()) {
  //   header += "Set-Cookie: Session-ID=" + session_->getID() +
  //             "; Max-Age=" + COOKIE_MAX_AGE + "; HttpOnly;" + CRLF;
  // }
}

std::string ResponseGenerator::getConnectionHeader(HttpRequest &request,
                                                   HttpResponse &response) {
  const std::string header_name = "Connection: ";
  if (response.isSuccessCode() == false) {
    return (header_name + "close");
  }
  if (request.getHeader("CONNECTION").empty() == false) {
    return (header_name + request.getHeader("CONNECTION"));
  }
  return (header_name + "keep-alive");
}

std::string ResponseGenerator::getDateHeader(void) {
  const std::string header_name = "Date: ";
  return (header_name + formatTime("%a, %d %b %Y %H:%M:%S GMT"));
}

std::string ResponseGenerator::getTransferEncodingHeader(HttpRequest &request) {
  if (request.getHeader("Transfer-Encoding").empty() == false) {
    return ("Transfer-Encoding: " + request.getHeader("Transfer-Encoding") +
            CRLF);
  }
  return "";
}

/*==========================*/
//   process for resource   //
/*==========================*/

std::string ResponseGenerator::directoryListing(const std::string &url) {
  DIR *dir = opendir(url.c_str());
  if (!dir) {
    throw std::runtime_error(strerror(errno));
  }
  std::set<File> entries;
  struct dirent *entry;
  while ((entry = readdir(dir))) {
    std::string name = entry->d_name;
    if (name == "." || name == "..") continue;
    struct stat statbuf;
    if (stat((url + name).c_str(), &statbuf) == ERROR<int>()) {
      throw std::runtime_error(strerror(errno));
    }
    File file = {name, statbuf.st_mtime, statbuf.st_size};
    entries.insert(file);
  }
  closedir(dir);
  return DirectoryListingHtml::generate(entries);
}

std::string ResponseGenerator::readIndexFile(HttpResponse &response,
                                             const std::string &url) {
  Location location_block = response.getLocation();
  for (std::size_t i = 0; i < location_block.getIndex().size(); ++i) {
    try {
      return readFile(response, url + location_block.getIndex()[i]);
    } catch (FileOpenException &e) {
      continue;
    }
  }
  if (location_block.getAutoindex() == false) {
    throw FileOpenException();
  }
  return directoryListing(url);
}

std::string ResponseGenerator::readFile(HttpResponse &response,
                                        const std::string &uri) {
  std::string url;
  if (isDirectory(uri)) {
    url = (*uri.rbegin() == '/') ? uri : uri + '/';
    return readIndexFile(response, url);
  }
  return ::readFile(uri);
}

/*==========================*/
//           util           //
/*==========================*/

void ResponseGenerator::replaceNLtoCRLF(std::string &header) {
  size_t pos = 0;
  const std::string NL = "\n";

  while ((pos = header.find(NL, pos)) != std::string::npos) {
    header.replace(pos, NL.length(), CRLF);
    pos += CRLF.length();
  }
}
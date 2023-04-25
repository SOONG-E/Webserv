#include "StaticContentHandler.hpp"

struct Response StaticContentHandler::handle(Client *client) {
  struct Response response;

  response.body = generateBody(client);
  response.has_body = true;

  response.headers["content-length"] = toString(response.body.size());

  return response;
}

std::string StaticContentHandler::generateBody(Client *client) {
  std::string body;
  std::string uri = client->getFullUri();

  try {
    if (client->getRequest().getMethod() == METHODS[DELETE]) {
      deleteFile(client, uri);
    }
    if (client->isErrorCode() == true) {
      body = loadErrorPage(client->getHttpServer(), client->getStatus());
      return body;
    }
    body = readPage(client->getLocation(), uri);
  } catch (FileOpenException &e) {
    throw ResponseException(C404);
  } catch (std::exception &e) {
    throw ResponseException(C500);
  }
  return body;
}

std::string StaticContentHandler::readIndexFile(Location &location,
                                                const std::string &url) {
  for (std::size_t i = 0; i < location.getIndex().size(); ++i) {
    try {
      return readPage(location, url + location.getIndex()[i]);
    } catch (FileOpenException &e) {
      continue;
    }
  }
  throw FileOpenException();
}

std::string StaticContentHandler::readPage(Location &location,
                                           const std::string &uri) {
  std::string url;
  if (isDirectory(uri)) {
    url = (*uri.rbegin() == '/') ? uri : uri + '/';
    return readIndexFile(location, url);
  }
  return ::readFile(uri);
}

std::string StaticContentHandler::loadErrorPage(HttpServer *http_server,
                                                const int status) {
  if (http_server == NULL) {
    return ::readFile(DEFAULT_ERROR_PAGE);
  }
  std::string uri = http_server->getErrorPage(ResponseStatus::CODES[status]);
  if (uri.empty() == true) {
    try {
      std::string page =
          DEFAULT_ERROR_DIRECTORY + ResponseStatus::CODES[status] + ".html";
      return ::readFile(DEFAULT_ERROR_DIRECTORY +
                        ResponseStatus::CODES[status] + ".html");
    } catch (FileOpenException &e) {
      return ::readFile(DEFAULT_ERROR_PAGE);
    }
  }
  return ::readFile(uri);
}

void StaticContentHandler::deleteFile(Client *client, const std::string &uri) {
  if (unlink(uri.c_str()) == -1) {
    throw FileOpenException();
  }
  client->setStatus(C204);
}

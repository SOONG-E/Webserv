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
      body = loadErrorPage(client->getHttpServer(), client->getStatusInt());
      return body;
    }
    if (*uri.rbegin() == '/') {
      uri.pop_back();  // 수정!!!!
    }
    body = ::readFile(uri);
  } catch (FileOpenException &e) {
    client->setStatus(C404);
    throw;  // error handling
  } catch (std::exception &e) {
    throw;
  }
  return body;
}

std::string StaticContentHandler::loadErrorPage(HttpServer *http_server,
                                                const int status) {
  std::string url;
  std::string uri = http_server->getErrorPage(ResponseStatus::CODES[status]);

  return ::readFile(uri);
}

void StaticContentHandler::deleteFile(Client *client, const std::string &uri) {
  if (unlink(uri.c_str()) == -1) {
    throw FileOpenException();
  }
  client->setStatus(C204);
}

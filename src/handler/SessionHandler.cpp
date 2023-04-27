#include "SessionHandler.hpp"

bool SessionHandler::recognizeRequest(HttpRequest &request) {
  if (request.getMethod() != METHODS[POST]) {
    return false;
  } else if (request.getUri().find("login") == NPOS) {
    return false;
  }
  return true;
}

struct Response SessionHandler::handle(Client *client) {
  struct Response response;

  std::string id = getNewSessionId(client->getHttpServer());
  createSession(client, id);

  response.headers["Set-Cookie"] = SESSION_ID_FIELD + "=" + id;
  return response;
}

std::string SessionHandler::getNewSessionId(HttpServer *server) {
  std::string id;
  do {
    id = createSessionId();
  } while (server->isExistSessionId(id) == true);
  return id;
}

std::string SessionHandler::createSessionId(void) {
  srand(time(NULL));

  std::string id;
  id.reserve(16);
  for (int i = 0; i != 16; ++i) {
    id += 'a' + (rand() % 16);
  }
  return id;
}

void SessionHandler::createSession(Client *client, std::string &id) {
  const std::string &body = client->getRequest().getBody();

  const Session::ValueType &values = parseData(body);

  Session *session = new Session(id, values);
  client->getHttpServer()->addSession(id, session);

  client->setSession(session);
}

Session::ValueType SessionHandler::parseData(const std::string &data) {
  std::vector<std::string> pairs = split(data, "&");
  std::vector<std::string> pair;
  Session::ValueType values;

  for (std::vector<std::string>::iterator it = pairs.begin(); it != pairs.end();
       ++it) {
    pair = split(*it, "=");
    if (pair.size() != 2) {
      throw ResponseException(C400);
    }
    values[pair[0]] = pair[1];
  }
  return values;
}
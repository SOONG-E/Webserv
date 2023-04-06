#include "SessionHandler.hpp"

#include <cstdlib>
#include <ctime>

#include "utility.hpp"

SessionHandler::SessionHandler() { std::srand(std::time(NULL)); }

SessionHandler::SessionHandler(const SessionHandler& src)
    : sessions_(src.sessions_) {}

SessionHandler& SessionHandler::operator=(const SessionHandler& src) {
  if (this != &src) {
    sessions_ = src.sessions_;
  }
  return *this;
}

SessionHandler::~SessionHandler() {}

Session* SessionHandler::generateSession(Client& client) {
  int server_block_key = client.getServerBlockKey();
  std::string session_key = client.getSessionKey();
  std::string session_id = generateSessionID(server_block_key);

  sessions_[server_block_key].insert(
      std::make_pair(session_key, Session(session_id, &client)));
  return &sessions_[server_block_key].find(session_key)->second;
}

Session* SessionHandler::findSession(const Client& client) {
  int server_block_key = client.getServerBlockKey();
  std::string session_key = client.getSessionKey();

  sessions_mapped_type& sessions = sessions_[server_block_key];
  try {
    Session& session = sessions.at(session_key);
    std::string session_id = client.getRequestObj().getCookie("Session-ID");
    if (session.getID() == session_id) {
      session.setTimeout();
      return &session;
    }
    Client* client = session.getClient();
    if (client) {
      client->getResponseObj().setSession(NULL);
    }
    sessions.erase(session_key);
  } catch (const std::out_of_range& e) {
  }
  return NULL;
}

void SessionHandler::deleteTimeoutSessions() {
  Session* session;
  std::queue<std::string> delete_sessions;

  for (sessions_type::iterator sessions_it = sessions_.begin();
       sessions_it != sessions_.end(); ++sessions_it) {
    for (sessions_mapped_type::iterator mapped_it = sessions_it->second.begin();
         mapped_it != sessions_it->second.end(); ++mapped_it) {
      session = &mapped_it->second;
      if (session->getTimeout() < std::time(NULL)) {
        std::string path = "upload_file/" + toString(sessions_it->first) + "/" +
                           session->getID();
        removeDirectory(path);
        delete_sessions.push(mapped_it->first);
        Client* client = session->getClient();
        if (client) {
          client->getResponseObj().setSession(NULL);
        }
      }
    }
    deleteSessions(sessions_it->second, delete_sessions);
  }
}

std::string SessionHandler::generateSessionID(int server_block_key) {
  std::string session_id;

  do {
    session_id = toString(std::rand());
  } while (isDuplicatedId(server_block_key, session_id));
  return session_id;
}

bool SessionHandler::isDuplicatedId(int server_block_key,
                                    const std::string& session_id) {
  for (sessions_mapped_type::iterator it = sessions_[server_block_key].begin();
       it != sessions_[server_block_key].end(); ++it) {
    if (it->second.getID() == session_id) {
      return true;
    }
  }
  return false;
}

void SessionHandler::deleteSessions(sessions_mapped_type& sessions,
                                    std::queue<std::string>& delete_sessions) {
  while (!delete_sessions.empty()) {
    sessions.erase(delete_sessions.front());
    delete_sessions.pop();
  }
}